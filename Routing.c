#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct binaryTreeStruct{
    char *routeName;
    char *routePath;
    struct binaryTreeStruct *left, *right;
};

struct binaryTreeStruct * createBinaryTree(char* routeName, char* routePath){
    struct binaryTreeStruct *node = (struct binaryTreeStruct * )malloc(sizeof(struct binaryTreeStruct *));
    node->routeName = (char *)malloc(sizeof(routeName));
    node->routePath = (char *)malloc(sizeof(routePath));
    if(node == NULL || node->routeName == NULL || node->routePath == NULL){
        printf("Memory allocation patladi");
        exit(1);
    }
    strcpy(node->routeName, routeName);
    strcpy(node->routePath, routePath);
    node->left = node->right = NULL;
    return node;
}

struct binaryTreeStruct * searchRoute(struct binaryTreeStruct *root, char *routeName){
    if(root == NULL || strcmp(root->routeName,routeName) == 0) {
        return root;
    }
    if(strcmp(root->routeName, routeName) > 0) {
        return searchRoute(root->right, routeName);
    }
    return searchRoute(root->left, routeName);
};

struct binaryTreeStruct * addRoute(struct binaryTreeStruct *node, char *routeName, char* routePath) {
    if(node == NULL) {
        return createBinaryTree(routeName, routePath);
    }
    if(strcmp(node->routeName, routeName) == 0){
        printf("----------BRUH NOO-----------\n");
        printf("There is already route assigned to this place");
    } else if(strcmp(node->routeName, routeName) > 0) {
        node->right = addRoute(node->right, routeName, routePath);
    } else if(strcmp(node->routeName, routeName) < 0){
        node->left = addRoute(node->left, routeName, routePath);
    }
    return node;
};

void postOrder(struct binaryTreeStruct *node) {
    if(node != NULL) {
        postOrder(node->left);
        postOrder(node->right);
        printf("Here is the routes: %s and their paths: %s\n", node->routeName, node->routePath);
    } 
}

void freeBinaryTree(struct binaryTreeStruct *node) {
    if (node == NULL) {
        return;
    }
    freeBinaryTree(node->left);
    freeBinaryTree(node->right);
    free(node->routeName);
    free(node->routePath);
    free(node);
}