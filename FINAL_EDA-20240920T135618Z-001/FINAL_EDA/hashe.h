#ifndef HASHE_H
#define HASHE_H

#include <iostream>
#include <list>
#include <vector>

// Estrutura para representar um par chave-valor na tabela hash
template<typename Key, typename Value>
struct henode {
    Key key;
    Value value;

    henode(Key k, Value v) : key(k), value(v) {}
};

// Classe para representar a tabela hash com endereçamento externo e rehashing
template<typename Key, typename Value>
class hash_table_e {
public:
    hash_table_e(size_t size = 10) 
        : table_size(size), num_elements(0), load_factor_threshold(0.75) {
        table.resize(table_size);
    }

    ~hash_table_e() {
        clear();
    }

    // Função para adicionar um par chave-valor à tabela
    void add(const Key& key, const Value& value) {
        if ((num_elements + 1) > table_size * load_factor_threshold) {
            rehash(table_size * 2); // Duplicar o tamanho da tabela
        }

        size_t hash_index = hash_function(key);
        auto& chain = table[hash_index];

        // Verifica se a chave já existe e atualiza o valor
        for (auto& node : chain) {
            if (node.key == key) {
                node.value = value;
                return;
            }
        }

        // Insere um novo nó na lista
        chain.emplace_back(key, value);
        ++num_elements;
    }

    // Função para verificar se uma chave está presente na tabela
    bool contains(const Key& key) const {
        size_t hash_index = hash_function(key);
        const auto& chain = table[hash_index];

        for (const auto& node : chain) {
            if (node.key == key) {
                return true;
            }
        }

        return false;
    }

    // Função para acessar o valor associado a uma chave
    Value& at(const Key& key) {
        size_t hash_index = hash_function(key);
        auto& chain = table[hash_index];

        for (auto& node : chain) {
            if (node.key == key) {
                return node.value;
            }
        }

        throw std::runtime_error("Chave não encontrada.");
    }

    // Função para remover um par chave-valor da tabela
    bool remove(const Key& key) {
        size_t hash_index = hash_function(key);
        auto& chain = table[hash_index];

        // Busca a chave e remove o nó correspondente
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->key == key) {
                chain.erase(it);
                --num_elements;
                return true;
            }
        }

        return false;
    }

    // Função para exibir o conteúdo da tabela
    std::vector<std::pair<Key, Value>> display() const {
        std::vector<std::pair<Key, Value>> vec;
        for (size_t i = 0; i < table_size; ++i) {
            for (const auto& node : table[i]) {
                vec.push_back({node.key, node.value});
            }
        }
        return vec;
    }

    // Função para limpar a tabela
    void clear() {
        for (auto& chain : table) {
            chain.clear();
        }
        num_elements = 0;
    }

private:
    std::vector<std::list<henode<Key, Value>>> table;
    size_t table_size;
    size_t num_elements;
    const double load_factor_threshold;

    // Função de hash simples para calcular o índice
    size_t hash_function(const Key& key) const {
        return std::hash<Key>{}(key) % table_size;
    }

    // Função para redimensionar a tabela quando necessário
    void rehash(size_t new_size) {
        std::vector<std::list<henode<Key, Value>>> new_table(new_size);

        // Reinsere todos os elementos na nova tabela
        for (auto& chain : table) {
            for (auto& node : chain) {
                size_t new_index = std::hash<Key>{}(node.key) % new_size;
                new_table[new_index].emplace_back(node.key, node.value);
            }
        }

        table = std::move(new_table);
        table_size = new_size;
    }
};

#endif
