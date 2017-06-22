#include <cstdio>
#include <list>

enum { MAX_WORDS_QT    = 1000 };
enum { HASH_TABLE_SIZE = 257  };

int hash1(const char* s);
int hash2(const char* s);
int hash3(const char* s);
int hash4(const char* s);
int hash5(const char* s);

int  get_file_size            (FILE*       f);
int  get_words_qt             (char*       str);
void make_words_ptrs          (char*       buf,        char** word_ptr);
void delete_punctuation_marks (char**      word_ptr,   int    words_qt);
void Hash_Hash                (char*       buff,       int    hash_function(const char*));
bool sort_words               (char*       first,      char*  second);
bool similar_words            (char*       first,      char*  second);
void strings_to_lower         (char**      word_ptr,   int    words_qt);
void make_graf                (std::list<char*> ListArray[]);
void delete_similar_words     (std::list<char*> ListArray[]);
