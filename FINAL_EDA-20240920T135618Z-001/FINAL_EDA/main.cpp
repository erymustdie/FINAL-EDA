#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <codecvt>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/uchar.h>
#include <unicode/locid.h>
#include "avltree.h"
#include "rbtree.h"
#include "hashl.h"
#include "hashe.h"

using namespace std;

// Estrutura que armazena a palavra original e normalizada (sem acentos)
class WordPair {
public:
    string without_accents;
    string with_accents;

    WordPair(string no_accents, string acc) : without_accents(no_accents), with_accents(acc) {}

    WordPair() : without_accents(""), with_accents("") {}

    bool operator>(const WordPair& other) const {
        return without_accents > other.without_accents;
    }

    bool operator<(const WordPair& other) const {
        return without_accents < other.without_accents;
    }

    bool operator==(const WordPair& other) const {
        return without_accents == other.without_accents;
    }

    bool operator!=(const WordPair& other) const {
        return without_accents != other.without_accents;
    }
};

// Sobrecarga do operador de inserção em stream para a classe WordPair
ostream& operator<<(ostream& os, const WordPair& pair) {
    os << pair.with_accents;
    return os;
}

namespace std {
    template<>
    struct hash<WordPair> {
        size_t operator()(const WordPair& wp) const {
            // Hash baseado na string 'without_accents'
            return hash<std::string>()(wp.without_accents);
        }
    };
}

// Função hash personalizada para a classe WordPair
class WordPairHash {
public:
    size_t operator()(const WordPair& word) const {
        return hash<string>{}(word.without_accents);
    }
};

// Classe que remove acentuação e processa palavras
class WordProcessor {
public:
    static icu::UnicodeString remove_accentuation(const icu::UnicodeString& input) {
        UErrorCode errorCode = U_ZERO_ERROR;
        const icu::Normalizer2* normalizer = icu::Normalizer2::getNFDInstance(errorCode);
        
        icu::UnicodeString decomposed;
        normalizer->normalize(input, decomposed, errorCode);

        icu::UnicodeString no_accents;
        for (int i = 0; i < decomposed.length(); ++i) {
            UChar32 c = decomposed.char32At(i);
            if (!u_hasBinaryProperty(c, UCHAR_DIACRITIC)) {
                no_accents.append(c);
            }
        }
        return no_accents;
    }

    static WordPair create_word_pair(const icu::UnicodeString& unicode_word) {
        icu::Locale locale("pt_BR");
        icu::UnicodeString lowercased_word = unicode_word; // Cria uma cópia mutável
        lowercased_word.toLower(locale); // Chama toLower() na cópia
        string with_accents;
        lowercased_word.toUTF8String(with_accents);

        icu::UnicodeString word_without_accents = remove_accentuation(lowercased_word);
        string without_accents;
        word_without_accents.toUTF8String(without_accents);

        return WordPair(without_accents, with_accents);
    }
};

// Função para processar o texto e contar a frequência das palavras
template <typename Dictionary>
void process_text(Dictionary& dictionary, fstream& input_file) {
    string line;
    while (getline(input_file, line)) {
        icu::UnicodeString unicode_line = icu::UnicodeString::fromUTF8(line);
        icu::UnicodeString current_word;

        for (int32_t i = 0; i < unicode_line.length(); ++i) {
            UChar32 character = unicode_line.char32At(i);
            if (u_isalpha(character) || (character == '-' && current_word.length() > 0)) {
                current_word.append(character);
            } else if (current_word.length() > 0) {
                if (current_word.char32At(current_word.length() - 1) == '-') {
                    current_word.truncate(current_word.length() - 1);
                }
                WordPair word_pair = WordProcessor::create_word_pair(current_word);
                if (dictionary.contains(word_pair)) {
                    dictionary.at(word_pair) += 1;
                } else {
                    dictionary.add(word_pair, 1);
                }
                current_word.remove(); // Limpa a palavra para o próximo uso
            }
        }

        if (current_word.length() > 0) {
            if (current_word.char32At(current_word.length() - 1) == '-') {
                current_word.truncate(current_word.length() - 1);
            }
            WordPair word_pair = WordProcessor::create_word_pair(current_word);
            if (dictionary.contains(word_pair)) {
                dictionary.at(word_pair) += 1;
            } else {
                dictionary.add(word_pair, 1);
            }
        }
    }
}

bool comparator_word_pair(pair<WordPair, int> w1, pair<WordPair, int> w2){
    return w1.first < w2.first;
}

int main(int argc, char* argv[]) {
    // Configurações de localidade
    std::setlocale(LC_ALL, "pt_BR.UTF-8");

    // Verifica se os argumentos são válidos
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <arquivo_entrada> <opção_estrutura>" << endl;
        cerr << "Opções de estrutura: 1 - AVL, 2 - Rubro-Negra, 3 - Hash Encadeado, 4 - Hash Linear" << endl;
        return 1;
    }

    // Abre o arquivo de entrada
    fstream input_file(argv[1]);
    ofstream saida("finalcountdown.txt");
    if (!input_file) {
        cerr << "Erro ao abrir o arquivo!" << std::endl;
        return 1;
    }

    // Inicializa a estrutura de dados com base na entrada do usuário
    int opcao = stoi(argv[2]);

    auto start_time = chrono::high_resolution_clock::now();

    vector<pair<WordPair, int>> vec;

    switch(opcao) {
        case 1: {
            // Árvore AVL
            avl_tree<WordPair, int> avl_dict;
            process_text(avl_dict, input_file);
            std::cout << "Usando Árvore AVL" << endl;
            vec = avl_dict.inorder_traversal();
            break;
        }
        case 2: {
            // Árvore Rubro-Negra
            red_black_tree<WordPair, int> rb_dict;
            process_text(rb_dict, input_file);
            std::cout << "Usando Árvore Rubro-Negra" << endl;
            vec = rb_dict.inorder_traversal();
            break;
        }
        case 3: {
            // Tabela Hash com Encadeamento Externo
            hash_table_e<WordPair, int> hashe_dict(19);
            process_text(hashe_dict, input_file);
            std::cout << "Usando Tabela Hash Encadeada" << endl;
            vec = hashe_dict.display();
            break;
        }
        case 4: {
            // Tabela Hash com Endereçamento Aberto
            hash_table_l<WordPair, int> hashl_dict(19);
            process_text(hashl_dict, input_file);
            std::cout << "Usando Tabela Hash com Endereçamento Aberto" << endl;
            vec = hashl_dict.display();
            break;
        }
        default: {
            cerr << "Opção de estrutura inválida! Escolha 1 (AVL), 2 (Rubro-Negra), 3 (Hash Encadeado) ou 4 (Hash Linear)." << endl;
            return 1;
        }
    }

    // Fim da medição de tempo
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    std::cout << "Tempo de processamento: " << duration.count() << " ms" << endl;

    sort(vec.begin(), vec.end(), comparator_word_pair);

    for(int i = 0; i < vec.size(); ++i){
        saida << "(" << vec[i].first << ", " << vec[i].second << ")\n";
    }
    // Fecha o arquivo de entrada
    input_file.close();

    return 0;
}
