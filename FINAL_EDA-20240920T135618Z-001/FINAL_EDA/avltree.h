#ifndef AVLTREE_H
#define AVLTREE_H

#include <iostream>

// Estrutura para representar um nó da árvore AVL
template<typename Key, typename Value>
struct avlnode {
    Key key;
    Value value;
    int height;
    avlnode* left;
    avlnode* right;

    avlnode(Key k, Value v)
        : key(k), value(v), height(1), left(nullptr), right(nullptr) {}
};

// Classe para representar a árvore AVL
template<typename Key, typename Value>
class avl_tree {
public:
    avl_tree() : root(nullptr) {}

    ~avl_tree() {
        delete_tree(root);
    }

    // Função para adicionar uma chave e valor à árvore
    void add(const Key& key, const Value& value) {
        root = insert_rec(root, key, value);
    }

    // Função para verificar se uma chave está presente na árvore
    bool contains(const Key& key) const {
        return search(key) != nullptr;
    }

    // Função para acessar o valor associado a uma chave
    Value& at(const Key& key) {
        avlnode<Key, Value>* result = search(key);
        if (result) {
            return result->value;
        }
        throw std::runtime_error("Chave não encontrada.");
    }

    // Função de percurso em ordem
    std::vector<std::pair<Key, Value>> inorder_traversal() const {
        std::vector<std::pair<Key, Value>>* vec = new std::vector<std::pair<Key, Value>>();
        inorder_rec(root, vec);
        return *vec;
    }

private:
    avlnode<Key, Value>* root;

    // Função auxiliar para obter a altura de um nó
    int get_height(avlnode<Key, Value>* n) const {
        return n ? n->height : 0;
    }

    // Função auxiliar para obter o fator de balanceamento de um nó
    int get_balance(avlnode<Key, Value>* n) const {
        return n ? get_height(n->left) - get_height(n->right) : 0;
    }

    // Função auxiliar para realizar rotação à direita
    avlnode<Key, Value>* right_rotate(avlnode<Key, Value>* y) {
        avlnode<Key, Value>* x = y->left;
        avlnode<Key, Value>* t2 = x->right;

        x->right = y;
        y->left = t2;

        y->height = std::max(get_height(y->left), get_height(y->right)) + 1;
        x->height = std::max(get_height(x->left), get_height(x->right)) + 1;

        return x;
    }

    // Função auxiliar para realizar rotação à esquerda
    avlnode<Key, Value>* left_rotate(avlnode<Key, Value>* x) {
        avlnode<Key, Value>* y = x->right;
        avlnode<Key, Value>* t2 = y->left;

        y->left = x;
        x->right = t2;

        x->height = std::max(get_height(x->left), get_height(x->right)) + 1;
        y->height = std::max(get_height(y->left), get_height(y->right)) + 1;

        return y;
    }

    // Função auxiliar para inserir um nó na árvore AVL
    avlnode<Key, Value>* insert_rec(avlnode<Key, Value>* n, const Key& key, const Value& value) {
        if (!n) {
            return new avlnode<Key, Value>(key, value);
        }

        if (key < n->key) {
            n->left = insert_rec(n->left, key, value);
        } else if (key > n->key) {
            n->right = insert_rec(n->right, key, value);
        } else {
            return n; // Chaves duplicadas não são permitidas
        }

        n->height = 1 + std::max(get_height(n->left), get_height(n->right));

        int balance = get_balance(n);

        if (balance > 1 && key < n->left->key) {
            return right_rotate(n);
        }

        if (balance < -1 && key > n->right->key) {
            return left_rotate(n);
        }

        if (balance > 1 && key > n->left->key) {
            n->left = left_rotate(n->left);
            return right_rotate(n);
        }

        if (balance < -1 && key < n->right->key) {
            n->right = right_rotate(n->right);
            return left_rotate(n);
        }

        return n;
    }

    // Função auxiliar para realizar o percurso em ordem
    void inorder_rec(avlnode<Key, Value>* n, std::vector<std::pair<Key, Value>>* vec) const {
        if (n) {
            inorder_rec(n->left, vec);
            vec->push_back({n->key, n->value});
            inorder_rec(n->right, vec);
        }
    }

    // Função auxiliar para buscar um nó na árvore
    avlnode<Key, Value>* search(avlnode<Key, Value>* n, const Key& key) const {
        if (!n || n->key == key) {
            return n;
        }

        if (key < n->key) {
            return search(n->left, key);
        } else {
            return search(n->right, key);
        }
    }

    // Função pública de busca que usa a função de busca auxiliar
    avlnode<Key, Value>* search(const Key& key) const {
        return search(root, key);
    }

    // Função auxiliar para deletar toda a árvore
    void delete_tree(avlnode<Key, Value>* n) {
        if (n) {
            delete_tree(n->left);
            delete_tree(n->right);
            delete n;
        }
    }
};

#endif
