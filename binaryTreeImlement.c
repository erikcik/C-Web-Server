#include <stdio.h>
#include <stdlib.h>

struct binaryTreeStruct {
    int key;
    struct binaryTreeStruct *left, *right;
};

struct binaryTreeStruct * createNewBinaryTree(int value) {
    struct binaryTreeStruct *temp = (struct binaryTreeStruct *)malloc(sizeof(struct binaryTreeStruct *));
    temp->key = value;
    temp->left = temp->right = NULL;
    return temp;
}

struct binaryTreeStruct * searchTree(struct binaryTreeStruct *root, int key) {
    if(root == NULL ||  root->key == key) {
        return root;
    }
    if(key > root->key) {
        return searchTree(root->right, key);
    }
    return searchTree(root->left, key);
}

struct binaryTreeStruct * insertNode(struct binaryTreeStruct *node, int key) {
    if(node == NULL) {
        return createNewBinaryTree(key);
    }
    if(key > node->key){
        node->right = insertNode(node->right, key);
    } else if(key < node->key) {
        node->left = insertNode(node->left, key);
    }
    //
    return node;
}
void postOrder(struct binaryTreeStruct* root) {
    if(root != NULL) {
        postOrder(root->left);
        postOrder(root->right);
        printf("%d\n", root->key);
    }
}


int main(int argc, char *argv[]){

    struct binaryTreeStruct* root = NULL;
    root = insertNode(root, 50);
    insertNode(root, 30);
    insertNode(root, 20);
    insertNode(root, 40);
    insertNode(root, 70);
    insertNode(root, 60);
    insertNode(root, 80);
 
    // Search for a node with key 60
    if (searchTree(root, 60) != NULL) {
        printf("60 found");
    }
    else {
        printf("60 not found");
    } 
    printf("\n");
    postOrder(root);
    
}
