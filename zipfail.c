#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

// makros
#define BS 4096	
#define BST_IDX BS
#define STR_ENCODE 2	
#define UP_LIMIT 18

unsigned long int text_size = 0, code_size = 0, print_count = 0;
unsigned char text_buf[BS + UP_LIMIT - 1];
int match_pos, match_len, input, output; 
int l_child[BS + 1], r_child[BS + 257], parent[BS + 1];

void tree() {
	for(int i=0; i<BS; i++) {
		parent[i] = BST_IDX;
	}
	for(int i = BS+1; i < BS+257; i++) {
		r_child[i] = BST_IDX;
	}
}

void new_node(int x) {
	unsigned char *key = &text_buf[x];
	int p = BS + 1 + key[0], cmp = 1;

	l_child[x] = BST_IDX;
	r_child[x] = BST_IDX;
	match_len = 0;

	while(1) {
		if(cmp >= 0) {
			if(r_child[p] != BST_IDX) {
				p = r_child[p];
			} else {
				r_child[p] = x; 
				parent[x] = p;
				return;
			}
		} else {
			if(l_child[p] != BST_IDX) {
				p = l_child[p];
			} else {
				l_child[p] = x; 
				parent[x] = p;
				return;
			}
		}
		
		int i;
		for(i=1; i<UP_LIMIT; i++) {
			if((cmp = key[i] - text_buf[p+i]) != 0) {
				break;
			}
		}

		if(i > match_len) {
			match_pos = p;
			if((match_len = i) >= UP_LIMIT) {
				break;
		}
	}

	l_child[x] = l_child[p];
	r_child[x] = r_child[p];
	parent[x] = parent[p];
	parent[l_child[p]] = x; 
	parent[r_child[p]] = x;

	if(r_child[parent[p]] == p) {
		r_child[parent[p]] = x;
	} else {
		l_child[parent[p]] = x;
	}
	parent[p] = BST_IDX;
}

void rmv_node(int x) {
	int q;

	if(parent[x] == BST_IDX) {
		return;
	}
	if(r_child[x] == BST_IDX) {
		q = l_child[x];
	} else if(l_child[x] == BST_IDX) {
		q = r_child[x];
	} else {
		q = l_child[x];
		if(r_child[q] != BST_IDX) {
			do {
				q = r_child[q];
			} while(r_child[q] != BST_IDX);
	
			r_child[parent[q]] = l_child[q]; 
			parent[l_child[q]] = parent[q];
			l_child[q] = l_child[x];
			parent[l_child[p]] = q;
		}
		r_child[q] = r_child[x];
		parent[r_child[x]] = q;
	}
	parent[q] = parent[x];

	if(r_child[parent[x]] == x) {
		r_child[parent[x]] = q;
	} else {
		l_child[parent[x]] = q;
	}
	parent[x] = BST_IDX;
}

void encode_str() {
	unsigned char code_buf[17], mask, chr;
	int len, x, i, str, last_match_len, cbuf_ptr;

	tree();
	code_buf[0] = 0;
	cbuf_ptr = mask = 1;
	str = 0; 
	x = BS - UP_LIMIT;

	for(i = str; i < x; i++) {
		text_buf[i] = 0;
	} 
	for(len = 0; len < UP_LIMIT && (read(input, &chr, 1) != 0); len++) {	
		text_buf[x + len] = chr;
	}
	if((text_size = len) == 0) {
		return;
	}
	for(i=1; i<=UP_LIMIT; i++) {
		new_node(x - i);
	}
	new_node(x);

	do {
		if(match_len > len) {
			match_len = len;
		}

		if(match_len <= STR_ENCODE) {
			match_len = 1;
			code_buf[0] |= mask;
			code_buf[cbuf_ptr++] = text_buf[x];
		} else {
			code_buf[cbuf_ptr++] = (unsigned char) match_pos;
			code_buf[cbuf_ptr++] = (unsigned char) (((match_pos >> 4) & 0xf0) | (match_len - (STR_ENCODE + 1)));
		}

		if((mask <<= 1) == 0) {
			for(i=0; i < cbuf_ptr; i++) {
				write(output, &code_buf[i], 1);
			}
			code_size = code_size + cbuf_ptr;
			code_buf[0] = 0;
			cbuf_ptr = 1;
			mask = 1;
		}
		last_match_len = match_len;

		for(i=0; i < last_match_len && (read(input, &chr, 1) != 0); i++) {
			rmv_node(str);
			text_buf[str] = chr;
			
			if(str < UP_LIMIT - 1) {
				text_buf[str + BS] = chr;
			}
			str = (str+1) & (BS-1);
			x = (x+1) & (BS-1);
			new_node(x);
		}

		if((text_size = text_size + i) > print_count) {
			print_count = print_count + 1024;
		}

		while(i++ < last_match_len) {
			rmv_node(str);
			str = (str+1) & (BS-1);
			x = (x+1) & (BS-1);

			if(--len) {
				new_node(x);
			}
		}
	} while(len > 0);

	if(cbuf_ptr > 1) {
		for(i=0; i<cbuf_ptr; i++) {
			write(output, &code_buf[i], 1);
		}
		code_size = code_size + cbuf_ptr;
	}
}
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf(2, "arguments not enough\n");
		exit();
	}

	if(strcmp("-m", argv[1]) == 0) {
		if((output = open(argv[2], O_WRONLY | O_CREATE)) < -1) {
			printf(2, "zip: cant open file %s\n", argv[2]);
			close(output);
			exit();
		}
		for(int i=3; i < argc; i++) {
			if(strcmp(argv[1], argv[i]) == 0) {
				printf(2, "zip: cant zip the same file\n");
				exit();
			}
			if((input = open(argv[i], O_RDONLY)) < -1) {
				printf(2, "zip: cant open file %s\n", argv[i]);
				close(input);
				exit();
			}
			encode_str();
			close(input);
			if(unlink(argv[i]) < 0) {
				printf(2, "zip: %s cant delete file\n", argv[i]);
				break;
			}
		}
		close(output);
	}
	else if(strcmp("-r", argv[1]) != 0 && strcmp("-m", argv[1]) != 0) {
		if((output = open(argv[1], O_WRONLY | O_CREATE)) < -1) {
			printf(2, "zip: cant open file %s\n", argv[1]);
			close(output);
			exit();
		}
		for(int i=2; i < argc; i++) {
			if(strcmp(argv[1], argv[i]) == 0) {
				printf(2, "zip: cant zip the same file\n");
				exit();
			}
			if((input = open(argv[i], O_RDONLY)) < -1) {
				printf(2, "zip: cant open file %s\n", argv[i]);
				close(input);
				exit();
			}
			encode_str();
			close(input);
		}
		close(output);
	}
	exit();
}
