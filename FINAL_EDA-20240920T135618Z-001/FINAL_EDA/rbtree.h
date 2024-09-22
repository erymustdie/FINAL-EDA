#ifndef RBTREE_H
#define RBTREE_H

#include <iostream>

// Enum para representar as cores dos nós
enum class color { RED, BLACK };

// Estrutura para representar um nó da árvore
template<typename Key, typename Value>
struct rbnode {
    Key key;
    Value value;
    color node_color;
    rbnode* left;
    rbnode* right;
    rbnode* parent;

    rbnode(Key k, Value v, color c)
        : key(k), value(v), node_color(c), left(nullptr), right(nullptr), parent(nullptr) {}
};

// Classe para representar a árvore rubro-negra
template<typename Key, typename Value>
class red_black_tree {
public:
    red_black_tree() : root(nullptr) {}

    ~red_black_tree() {
        delete_tree(root);
    }

    // Função para adicionar uma chave e valor à árvore
    void add(const Key& key, const Value& value) {
        rbnode<Key, Value>* new_node = new rbnode<Key, Value>(key, value, color::RED);
        if (!root) {
            root = new_node;
            root->node_color = color::BLACK;
        } else {
            root = insert_rec(root, new_node);
            fix_insert(new_node);
        }
    }

    // Função para verificar se uma chave está presente na árvore
    bool contains(const Key& key) const {
        return search(key) != nullptr;
    }

    // Função para acessar o valor associado a uma chave
    Value& at(const Key& key) {
        rbnode<Key, Value>* result = search(key);
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
    rbnode<Key, Value>* root;

    // Função auxiliar para inserir um nó na árvore rubro-negra
    rbnode<Key, Value>* insert_rec(rbnode<Key, Value>* root, rbnode<Key, Value>* new_node) {
        if (!root) {
            return new_node;
        }

        if (new_node->key < root->key) {
            root->left = insert_rec(root->left, new_node);
            root->left->parent = root;
        } else if (new_node->key > root->key) {
            root->right = insert_rec(root->right, new_node);
            root->right->parent = root;
        }

        return root;
    }

    // Função para corrigir a árvore após a inserção de um novo nó
    void fix_insert(rbnode<Key, Value>* node) {
        while (node != root && node->parent->node_color == color::RED) {
            rbnode<Key, Value>* parent = node->parent;
            rbnode<Key, Value>* grandparent = parent ? parent->parent : nullptr;

            if (!grandparent) break; // Verificação adicional para evitar segmentation fault

            if (parent == grandparent->left) {
                rbnode<Key, Value>* uncle = grandparent->right;
                if (uncle && uncle->node_color == color::RED) {
                    grandparent->node_color = color::RED;
                    parent->node_color = color::BLACK;
                    uncle->node_color = color::BLACK;
                    node = grandparent;
                } else {
                    if (node == parent->right) {
                        node = parent;
                        rotate_left(node);
                    }
                    parent->node_color = color::BLACK;
                    grandparent->node_color = color::RED;
                    rotate_right(grandparent);
                }
            } else {
                rbnode<Key, Value>* uncle = grandparent->left;
                if (uncle && uncle->node_color == color::RED) {
                    grandparent->node_color = color::RED;
                    parent->node_color = color::BLACK;
                    uncle->node_color = color::BLACK;
                    node = grandparent;
                } else {
                    if (node == parent->left) {
                        node = parent;
                        rotate_right(node);
                    }
                    parent->node_color = color::BLACK;
                    grandparent->node_color = color::RED;
                    rotate_left(grandparent);
                }
            }
        }
        root->node_color = color::BLACK;
    }

    // Função auxiliar para realizar rotação à esquerda
    void rotate_left(rbnode<Key, Value>* node) {
        if (!node || !node->right) return; // Verificação adicional

        rbnode<Key, Value>* right_child = node->right;
        node->right = right_child->left;
        if (right_child->left) {
            right_child->left->parent = node;
        }

        right_child->parent = node->parent;
        if (!node->parent) {
            root = right_child;
        } else if (node == node->parent->left) {
            node->parent->left = right_child;
        } else {
            node->parent->right = right_child;
        }

        right_child->left = node;
        node->parent = right_child;
    }

    // Função auxiliar para realizar rotação à direita
    void rotate_right(rbnode<Key, Value>* node) {
        if (!node || !node->left) return; // Verificação adicional

        rbnode<Key, Value>* left_child = node->left;
        node->left = left_child->right;
        if (left_child->right) {
            left_child->right->parent = node;
        }

        left_child->parent = node->parent;
        if (!node->parent) {
            root = left_child;
        } else if (node == node->parent->right) {
            node->parent->right = left_child;
        } else {
            node->parent->left = left_child;
        }

        left_child->right = node;
        node->parent = left_child;
    }

    // Função auxiliar para realizar o percurso em ordem
    void inorder_rec(rbnode<Key, Value>* node, std::vector<std::pair<Key, Value>>* vec) const {
        if (node) {
            inorder_rec(node->left, vec);
            vec->push_back({node->key, node->value});
            inorder_rec(node->right, vec);
        }
    }

    // Função auxiliar para buscar um nó na árvore
    rbnode<Key, Value>* search_rec(rbnode<Key, Value>* node, const Key& key) const {
        if (!node) return nullptr; // Verificação adicional

        if (key < node->key) {
            return search_rec(node->left, key);
        } else if (key > node->key) {
            return search_rec(node->right, key);
        } else {
            return node;
        }
    }

    // Função pública de busca que usa a função de busca auxiliar
    rbnode<Key, Value>* search(const Key& key) const {
        return search_rec(root, key);
    }

    // Função auxiliar para deletar toda a árvore
    void delete_tree(rbnode<Key, Value>* node) {
        if (node) {
            delete_tree(node->left);
            delete_tree(node->right);
            delete node;
        }
    }
};

#endif
