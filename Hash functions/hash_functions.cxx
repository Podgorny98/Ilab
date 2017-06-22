#include "hash_functions.hxx"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <assert.h>

#include <list>
#include <iterator>

//======================================================================
//======================================================================
int mySuperHash(const char* s)
{
    int len = strlen(s);
    assert(len);
    return s[(len > 5) ? 5 : len - 1] % HASH_TABLE_SIZE;
}
//======================================================================
int main(int argc, char **argv) {
	FILE *f;
	if(argc == 1) {
        printf("Usage:\nusername$ ./Hash\\ table <file name>\n");
		exit(1);
	}
	f = fopen(argv[1], "r");

	if(f == NULL) {
		perror(argv[1]);
		exit(2);
	}
	int size = get_file_size(f);
	char* buff = (char*)malloc((size + sizeof('\0')) * sizeof(char));
	fread(buff, sizeof(char), size, f);
	buff[size] = '\0';

	Hash_Hash(buff, mySuperHash);

	fclose(f);
	free(buff);
	return 0;
}
//======================================================================
int get_file_size(FILE* f) {
	if(!f) {
		printf("Can't read file");
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	int i = ftell(f);
	rewind(f);
	return i;
}
//======================================================================
void Hash_Hash(char* buff, int hash_function(const char*)) {
	std::list<char*> ListArray[HASH_TABLE_SIZE] = {};
	int words_qt = get_words_qt(buff);
	char ** word_ptr = (char**)malloc(words_qt * sizeof(char*));
	make_words_ptrs(buff, word_ptr);
	delete_punctuation_marks(word_ptr, words_qt);
	strings_to_lower(word_ptr, words_qt);
	for(int i = 0; i < words_qt; i++)
		ListArray[hash_function(word_ptr[i])].push_back(word_ptr[i]);
	delete_similar_words(ListArray);
	make_graf(ListArray);
	free(word_ptr);
}
//======================================================================
int get_words_qt(char* str) {							//получает количество слов в тексте
	int res = 1;
	int len = strlen(str);
	for(int i = 0; i < len; i++) {
		if(str[i] == ' ' || str[i] == '\n')
			res++;
	}
	return res;
}
//======================================================================
void make_words_ptrs(char* buf, char** word_ptr) {		//формирует указатели на слова
	word_ptr[0] = buf;
	int i = 0;
	int len = strlen(buf);
	for(int k = 0; k < len; k++) {
		if((buf[k] == ' ') | (buf[k] == '\n')) {
			buf[k] = '\0';
			word_ptr[++i] = buf + k + 1;
		}
	}
}
//======================================================================
void delete_punctuation_marks(char** word_ptr, int words_qt) {		//удаляет знаки препинания в словах
	for(int i = 0; i < words_qt; i++) {
		int len = strlen(word_ptr[i]);
		char s = word_ptr[i][len - 1];
		if(s == '"') {
			word_ptr[i][len - 1] = '\0';
			s = word_ptr[i][len - 2];
		}
		if(s == '.' || s == ',' || s == ';' || s == '!' || s == '?')
			word_ptr[i][len - 1] = '\0';
		if(word_ptr[i][0] == '"')
			word_ptr[i]++;
	}
}
//======================================================================
void strings_to_lower(char** word_ptr, int words_qt) {		//делает все первые буквы слов строчными
	for(int i = 0; i < words_qt; i++)
		word_ptr[i][0] = tolower(word_ptr[i][0]);
}
//======================================================================
void delete_similar_words(std::list<char*> ListArray[]) {		//удаляет одинаковые слова в списке
	for(int i = 0; i < HASH_TABLE_SIZE; i++) {
		ListArray[i].sort(sort_words);
		ListArray[i].unique(similar_words);
	}
}

bool sort_words(char* first, char* second) {		//сравнивает слова по алфавиту
	return (strcmp(first, second) < 0);
}

bool similar_words(char* first, char* second) {		//проверяет равенство слов
	return !strcmp(first, second);
}
//======================================================================
void make_graf(std::list<char*> ListArray[]) {		//строит график кол-ва элем-в от № списка
	FILE* f_data = fopen("data.txt", "w");
	FILE* f_script = fopen("f_script", "w");
	fprintf(f_script, "set xrange [-1:258]\n"
				//"set yrange [-%u:%lu\n"
				"plot \"data.txt\" using 1:2 with boxes");
	for(int i = 0; i < HASH_TABLE_SIZE; i++)
		fprintf(f_data, "%d\t%d\n", i, (int)ListArray[i].size());
	fclose(f_data);
	fclose(f_script);
	system("gnuplot -persist  f_script"); 
}
