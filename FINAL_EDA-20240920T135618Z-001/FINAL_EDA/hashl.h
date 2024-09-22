#ifndef HASHL_H
#define HASHL_H

#include <iostream>
#include <vector>
#include <string>

// Enum para indicar o status de uma entrada na tabela hash
enum class entry_status { EMPTY, OCCUPIED, REMOVED };

// Estrutura para representar um par chave-valor na tabela hash
template<typename Key, typename Value>
struct hlnode {
    Key key;
    Value value;
    entry_status status;

    hlnode() : status(entry_status::EMPTY) {}

    hlnode(const Key& k, const Value& v)
        : key(k), value(v), status(entry_status::OCCUPIED) {}
};

// Classe para representar a tabela hash com endereçamento aberto e sondagem linear
template<typename Key, typename Value>
class hash_table_l {
public:
    hash_table_l(size_t size = 10)
        : table_size(size), num_elements(0), load_factor_threshold(0.75) {
        table.resize(table_size); // Inicializa com entradas vazias (EMPTY)
    }

    void add(const Key& key, const Value& value) {  // Renomeado de "insert" para "add"
        if ((num_elements + 1) > table_size * load_factor_threshold) {
            rehash(table_size * 2); // Duplicar o tamanho da tabela
        }

        size_t index = hash_function(key);

        // Encontrar o próximo índice disponível usando sondagem linear
        while (table[index].status == entry_status::OCCUPIED) {
            if (table[index].key == key) {
                // Atualiza o valor se a chave já existir
                table[index].value = value;
                return;
            }
            index = (index + 1) % table_size;
        }

        // Insere a nova entrada
        table[index] = hlnode<Key, Value>(key, value);
        ++num_elements;
    }

    bool contains(const Key& key) const {
        size_t index = hash_function(key);
        size_t start_index = index;

        // Percorre a tabela até encontrar a chave ou uma entrada vazia
        while (table[index].status != entry_status::EMPTY) {
            if (table[index].status == entry_status::OCCUPIED && table[index].key == key) {
                return true;
            }
            index = (index + 1) % table_size;
            if (index == start_index) { // Se voltarmos ao índice inicial, a chave não está na tabela
                break;
            }
        }

        return false;
    }

    Value& at(const Key& key) {
        size_t index = hash_function(key);
        size_t start_index = index;

        // Percorre a tabela até encontrar a chave ou uma entrada vazia
        while (table[index].status != entry_status::EMPTY) {
            if (table[index].status == entry_status::OCCUPIED && table[index].key == key) {
                return table[index].value;
            }
            index = (index + 1) % table_size;
            if (index == start_index) { // Se voltarmos ao índice inicial, a chave não está na tabela
                break;
            }
        }

        throw std::runtime_error("Chave não encontrada.");
    }

    bool search(const Key& key, Value& value) const {
        size_t index = hash_function(key);
        size_t start_index = index;

        // Percorre a tabela até encontrar a chave ou uma entrada vazia
        while (table[index].status != entry_status::EMPTY) {
            if (table[index].status == entry_status::OCCUPIED && table[index].key == key) {
                value = table[index].value;
                return true;
            }
            index = (index + 1) % table_size;
            if (index == start_index) { // Se voltarmos ao índice inicial, a chave não está na tabela
                break;
            }
        }

        return false;
    }

    bool remove(const Key& key) {
        size_t index = hash_function(key);
        size_t start_index = index;

        // Percorre a tabela até encontrar a chave ou uma entrada vazia
        while (table[index].status != entry_status::EMPTY) {
            if (table[index].status == entry_status::OCCUPIED && table[index].key == key) {
                table[index].status = entry_status::REMOVED; // Marca como removido
                --num_elements;
                return true;
            }
            index = (index + 1) % table_size;
            if (index == start_index) { // Se voltarmos ao índice inicial, a chave não está na tabela
                break;
            }
        }

        return false;
    }

    std::vector<std::pair<Key, Value>> display() const {
        std::vector<std::pair<Key, Value>> vec;
        for (size_t i = 0; i < table_size; ++i) {
            if (table[i].status == entry_status::OCCUPIED) {
                vec.push_back({table[i].key, table[i].value});
            } 
        }
        return vec;
    }

    void clear() {
        for (auto& entry : table) {
            entry.status = entry_status::EMPTY; // Marca todas as entradas como vazias
        }
        num_elements = 0;
    }

private:
    std::vector<hlnode<Key, Value>> table;
    size_t table_size;
    size_t num_elements;
    const double load_factor_threshold;

    size_t hash_function(const Key& key) const {
        // Função de hash simples, usando o operador % para definir o índice
        return std::hash<Key>{}(key) % table_size;
    }

    void rehash(size_t new_size) {
        std::vector<hlnode<Key, Value>> new_table(new_size);

        // Reinsere todos os elementos ativos na nova tabela
        for (auto& entry : table) {
            if (entry.status == entry_status::OCCUPIED) {
                size_t new_index = std::hash<Key>{}(entry.key) % new_size;
                while (new_table[new_index].status == entry_status::OCCUPIED) {
                    new_index = (new_index + 1) % new_size;
                }
                new_table[new_index] = entry; // Move a entrada para a nova tabela
            }
        }

        table = std::move(new_table);
        table_size = new_size;
    }
};

#endif
