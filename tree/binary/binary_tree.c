#include "binary_tree.h"
#include "../../stack/stack.h"

/**
 * 
 * @return BinaryTree
 */
BinaryTree newBinaryTree(){
    BinaryTree this_binary_tree;
    this_binary_tree.binary_tree_adt = NULL;
    this_binary_tree.number_elements = 0;
    this_binary_tree.depth = 0;
    this_binary_tree.empty = _empty_binary_tree;
    this_binary_tree.insert = _insert_binary_tree;
    this_binary_tree.insert_multiple = _insert_multiple_binary_tree;
    this_binary_tree.print = _print_binary_tree;
    this_binary_tree.find = _find_binary_tree;
    this_binary_tree.get = _get_binary_tree;
    return this_binary_tree;
}

/**
 * 
 * @param this_binary_tree 
 */
void destroyBinaryTree(BinaryTree *this_binary_tree){
    this_binary_tree->empty(this_binary_tree);
}

/**
 * 
 * @param this_binary_tree 
 * @param data_to_insert 
 * @param callback_insert 
 * @param callback_order 
 * @return 
 */
int _insert_binary_tree(BinaryTree *this_binary_tree, const void* data_to_insert, void(*callback_insert)(const void* d), int(*callback_order)(const void* new, const void* inserted) ){

    int r;
    //first element
    if( this_binary_tree->binary_tree_adt == NULL)
    {
        PBinaryTreeADT new;
        new = (PBinaryTreeADT)malloc(sizeof(ELEMENT_BINARY_TREE));
        if(callback_insert != NULL)
            callback_insert(data_to_insert);
        new->father = NULL;
        new->data = (void*)data_to_insert;
        new->right_leaf  = NULL;
        new->left_leaf  = NULL;
        new->repeat = 0;
        new->level = 0;
        this_binary_tree->binary_tree_adt = new;
        this_binary_tree->number_elements++;
        return 1;
    }else{
        PBinaryTreeADT tmp = this_binary_tree->binary_tree_adt;
        while(1){
            r = callback_order(data_to_insert, tmp->data);
            // less
            if( r == -1) {
                if( tmp->left_leaf != NULL){
                    tmp = (PBinaryTreeADT)tmp->left_leaf;
                }else{
                    PBinaryTreeADT new = (PBinaryTreeADT)malloc(sizeof(ELEMENT_BINARY_TREE));
                    if(callback_insert != NULL)
                        callback_insert(data_to_insert);
                    new->data = (void*)data_to_insert;
                    new->right_leaf  = NULL;
                    new->left_leaf  = NULL;
                    new->repeat = 0;
                    tmp->left_leaf = new;
                    new->father = tmp;
                    new->level = tmp->level + 1;
                    if(new->level > this_binary_tree->depth){
                        this_binary_tree->depth = new->level;
                    }
                    this_binary_tree->number_elements++;
                    return 1;
                }
            }
            // high
            else if( r == 1) {
                if( tmp->right_leaf != NULL){
                    tmp = tmp->right_leaf;
                }else{
                    PBinaryTreeADT new = (PBinaryTreeADT)malloc(sizeof(ELEMENT_BINARY_TREE));
                    if(callback_insert != NULL)
                        callback_insert(data_to_insert);
                    new->data = (void*)data_to_insert;
                    new->right_leaf  = NULL;
                    new->left_leaf  = NULL;
                    new->repeat = 0;
                    tmp->right_leaf = new;
                    new->father = tmp;
                    new->level = tmp->level + 1;
                    if(new->level > this_binary_tree->depth){
                        this_binary_tree->depth = new->level;
                    }
                    this_binary_tree->number_elements++;
                    return 1;
                }
            }
            // equal
            else if (r == 0) {
                tmp->repeat++;
                break;
            }
        }
        return 1;
    }

}

/**
 *
 * @param this_binary_tree
 * @param callback_insert
 * @param callback_order
 * @param count
 * @return
 */
int _insert_multiple_binary_tree(BinaryTree *this_binary_tree, void(*callback_insert)(const void* d), int(*callback_order)(const void* new, const void* inserted),  int count, ...){
    int i = 0;
    int r = 1;
    va_list lt;
    va_start(lt, count);

    for (i = 0;i<count;i++)
        r = r && this_binary_tree->insert( this_binary_tree, va_arg(lt, void*), callback_insert, callback_order);
    va_end(lt);

    return r;
}

/**
 *
 * @param this_binary_tree
 * @return
 */
ChainingGet _get_binary_tree(const BinaryTree *this_binary_tree){
    static ChainingGet methods;//Method chaining
    //DATA STACK
    typedef struct{ PBinaryTreeADT ptb; }struct_ptb;
    static const BinaryTree* b;//copy BinaryTree
    b = this_binary_tree;

    PreorderGet preorder() {
        static PreorderGet preorder;

        //NID: NODO RAIZ, NODO IZQUIERDO, NODO DERECHO
        void** left(void){
            if(b->number_elements > 0) {
                int i = 0;
                void ** datas = calloc(sizeof(void*), b->number_elements);
                static Stack stack1;
                stack1 = newStack();
                stack1.push(&stack1, b->binary_tree_adt, NULL);
                while (stack1.size > 0) {
                    PBinaryTreeADT pbinary_tree_adt1 = (PBinaryTreeADT)stack1.peek(&stack1);
                    stack1.pop(&stack1, NULL);
                    datas[i++] = pbinary_tree_adt1->data;
                    if (pbinary_tree_adt1->right_leaf != NULL) {
                        stack1.push(&stack1, (void *) pbinary_tree_adt1->right_leaf, NULL);
                    }
                    if (pbinary_tree_adt1->left_leaf != NULL) {
                        stack1.push(&stack1, (void *) pbinary_tree_adt1->left_leaf, NULL);
                    }
                }
                destroyStack(&stack1);
                return datas;
            }else
                return NULL;
        }

        //DIN: NODO DERECHO, NODOD IZQUIERDO, NODOD RAIZ
        void** right(void) {
            if (b->number_elements > 0) {
                int i = 0;
                void ** datas = calloc(sizeof(void*), b->number_elements);
                PBinaryTreeADT currentNode = NULL;
                static Stack stack1;
                stack1 = newStack();
                stack1.push(&stack1, b->binary_tree_adt, NULL);
                PBinaryTreeADT prev = NULL;

                while (stack1.size > 0) {
                    currentNode = (PBinaryTreeADT) stack1.peek(&stack1);
                    /* case 1. We are nmoving down the tree. */
                    if (prev == NULL || prev->left_leaf == currentNode || prev->right_leaf == currentNode) {
                        if (currentNode->right_leaf)
                            stack1.push(&stack1, currentNode->right_leaf, NULL);
                        else if (currentNode->left_leaf)
                            stack1.push(&stack1, currentNode->left_leaf, NULL);
                        else {
                            /* If node is leaf node */
                            datas[i++] = currentNode->data;
                            stack1.pop(&stack1, NULL);
                        }
                    }
                    /* case 2. We are moving up the tree from left child */
                    if (currentNode->right_leaf == prev) {
                        if (currentNode->left_leaf)
                            stack1.push(&stack1, currentNode->left_leaf, NULL);
                        else {
                            datas[i++] = currentNode->data;
                            stack1.pop(&stack1, NULL);
                        }
                    }
                    /* case 3. We are moving up the tree from right child */
                    if (currentNode->left_leaf == prev) {
                        datas[i++] = currentNode->data;
                        stack1.pop(&stack1, NULL);
                    }
                    prev = currentNode;
                }

                destroyStack(&stack1);
                return datas;
            }else return NULL;
        }
        preorder.left = left;
        preorder.right = right;
        return preorder;
    }

    InorderGet inorder() {
        static InorderGet inorder;

        //IND: NODO IZQUIERDO, NODO RAIZ, NODO DERECHO
        void** asc(void){
            if(b->number_elements > 0){
                int i = 0;
                void** datas = calloc(sizeof(void*), b->number_elements);
                static Stack stack1;
                stack1 = newStack();
                PBinaryTreeADT pbinary_tree_adt1 = b->binary_tree_adt;
                while(stack1.size > 0 || pbinary_tree_adt1 ){
                    if(pbinary_tree_adt1){
                        stack1.push(&stack1, pbinary_tree_adt1, NULL);
                        pbinary_tree_adt1 = (PBinaryTreeADT)pbinary_tree_adt1->left_leaf;
                    }
                    else {
                        pbinary_tree_adt1 = (PBinaryTreeADT)stack1.peek(&stack1);
                        stack1.pop(&stack1, NULL);
                        datas[i++] = pbinary_tree_adt1->data;
                        pbinary_tree_adt1 =(PBinaryTreeADT) pbinary_tree_adt1->right_leaf;
                    }
                }
                destroyStack(&stack1);
                return datas;
            }return NULL;
        }

        //DNI: NODO DERECHO, NODO RAIZ, NODO IZQUIERDO
        void** des(void){
            if(b->number_elements > 0){
                int i = 0;
                void **datas = calloc(sizeof(void*), b->number_elements);
                static Stack stack1;
                stack1 = newStack();
                PBinaryTreeADT pbinary_tree_adt1 = b->binary_tree_adt;
                while(stack1.size > 0 || pbinary_tree_adt1 ){
                    if(pbinary_tree_adt1){
                        stack1.push(&stack1, pbinary_tree_adt1, NULL);
                        pbinary_tree_adt1 = (PBinaryTreeADT)pbinary_tree_adt1->right_leaf;
                    }
                    else {
                        pbinary_tree_adt1 = (PBinaryTreeADT)stack1.peek(&stack1);
                        stack1.pop(&stack1, NULL);
                        datas[i++] = pbinary_tree_adt1->data;
                        pbinary_tree_adt1 =(PBinaryTreeADT) pbinary_tree_adt1->left_leaf;
                    }
                }
                destroyStack(&stack1);
                return datas;
            }else
                return NULL;
        }

        inorder.asc = asc;
        inorder.des = des;
        return inorder;
    }

    PostorderGet postorder() {
        static PostorderGet postorder;

        //IDN: NODO IZQUIERDO, NODO DERECHO, NODO RAIZ
        void** left(void){
            if(b->number_elements > 0) {
                int i = 0;
                void** datas = calloc(sizeof(void*), b->number_elements);
                PBinaryTreeADT currentNode = NULL;
                static Stack stack1;
                stack1 = newStack();
                stack1.push(&stack1, b->binary_tree_adt, NULL);
                PBinaryTreeADT prev = NULL;

                while (stack1.size > 0) {
                    currentNode = (PBinaryTreeADT) stack1.peek(&stack1);
                    /* case 1. We are nmoving down the tree. */
                    if (prev == NULL || prev->left_leaf == currentNode || prev->right_leaf == currentNode) {
                        if (currentNode->left_leaf)
                            stack1.push(&stack1, currentNode->left_leaf, NULL);
                        else if (currentNode->right_leaf)
                            stack1.push(&stack1, currentNode->right_leaf, NULL);
                        else {
                            /* If node is leaf node */
                            datas[i++] = currentNode->data;
                            stack1.pop(&stack1, NULL);
                        }
                    }
                    /* case 2. We are moving up the tree from left child */
                    if (currentNode->left_leaf == prev) {
                        if (currentNode->right_leaf)
                            stack1.push(&stack1, currentNode->right_leaf, NULL);
                        else {
                            datas[i++] = currentNode->data;
                            stack1.pop(&stack1, NULL);
                        }
                    }
                    /* case 3. We are moving up the tree from right child */
                    if (currentNode->right_leaf == prev) {
                        datas[i++] = currentNode->data;
                        stack1.pop(&stack1, NULL);
                    }
                    prev = currentNode;
                }
                destroyStack(&stack1);
                return datas;
            }else
                return NULL;
        }

        //NDI: NODO RAIZ, NODO DERECHO, NODO IZQUIERDO
        void** right(void){
            if(b->number_elements > 0) {
                int i = 0;
                void** datas = calloc(sizeof(void*), b->number_elements);
                static Stack stack1;
                stack1 = newStack();
                stack1.push(&stack1, b->binary_tree_adt, NULL);
                while (stack1.size > 0) {
                    PBinaryTreeADT pbinary_tree_adt1 = (PBinaryTreeADT) stack1.peek(&stack1);
                    stack1.pop(&stack1, NULL);
                    datas[i++] = pbinary_tree_adt1->data;
                    if (pbinary_tree_adt1->left_leaf != NULL)
                        stack1.push(&stack1, (void *) pbinary_tree_adt1->left_leaf, NULL);
                    if (pbinary_tree_adt1->right_leaf != NULL)
                        stack1.push(&stack1, (void *) pbinary_tree_adt1->right_leaf, NULL);
                }
                destroyStack(&stack1);
                return datas;
            }else
                return NULL;
        }

        postorder.left = left;
        postorder.right = right;
        return postorder;
    }


    methods.inorder = inorder;
    methods.postorder = postorder;
    methods.preorder = preorder;

    return methods;
}

/**
 *
 * @param this_binary_tree
 * @param d
 * @param callback
 * @return
 */
Find _find_binary_tree(BinaryTree *this_binary_tree, const void* d, int(*callback)(const void* d1, const void* d2)){
    Find find;

    static BinaryTree* b;//copy BinaryTree
    static int(*function)(const void* d1, const void* d2);
    static const void* data_to_find;


    b = this_binary_tree;
    function = callback;
    data_to_find = d;

    PBinaryTreeADT get(void){
        if(b->number_elements>0){
            PBinaryTreeADT pb = b->binary_tree_adt;
            int i = 0;
            while(pb != NULL){
                i = function(data_to_find, pb->data);
                //mayor
                if( i == 1 ){
                    pb = pb->right_leaf;
                }//menor
                else if( i == -1 ){
                    pb = pb->left_leaf;
                }
                //igual
                else if( i == 0){
                    return pb;
                }
            }
        }
        return NULL;
    }

    int remove(){
        if(b->number_elements>0){
            PBinaryTreeADT pb = b->binary_tree_adt;
            int i = 0;
            while(pb != NULL){
                i = function(data_to_find, pb->data);
                //mayor
                if( i == 1 ){
                    pb = pb->right_leaf;
                }//menor
                else if( i == -1 ){
                    pb = pb->left_leaf;
                }
                    //igual
                else if( i == 0){
                    //ningun hijo
                    if(pb->right_leaf != NULL && pb->left_leaf != NULL){
                        //raiz
                        if(pb->father == NULL){
                            free(pb);
                            b->binary_tree_adt = NULL;
                            b->number_elements = b->depth = 0;
                        }else{
                            if(pb->father->left_leaf == pb){
                                pb->father->left_leaf = NULL;
                            }else if(pb->father->right_leaf == pb){
                                pb->father->right_leaf = NULL;
                            }
                            free(pb);
                            b->number_elements = b->number_elements-1;
                        }
                    }//hijo izquierdo
                    else if(pb->right_leaf == NULL && pb->left_leaf != NULL){
                        if(pb->father->left_leaf == pb){
                            pb->father->left_leaf = pb->left_leaf;
                        }else if(pb->father->right_leaf == pb){
                            pb->father->right_leaf = pb->left_leaf;
                        }
                        free(pb);
                        b->number_elements = b->number_elements-1;

                    }//hijo derecho
                    else if(pb->right_leaf != NULL && pb->left_leaf == NULL){
                        if(pb->father->left_leaf == pb){
                            pb->father->left_leaf = pb->right_leaf;
                        }else if(pb->father->right_leaf == pb){
                            pb->father->right_leaf = pb->right_leaf;
                        }
                        free(pb);
                        b->number_elements = b->number_elements-1;
                    }//two children
                    else if(pb->right_leaf == NULL && pb->left_leaf == NULL){
                        PBinaryTreeADT tmp_left = pb->left_leaf;
                        PBinaryTreeADT tmp_right = pb->right_leaf;
                        while(tmp_left != NULL || tmp_right != NULL) {
                            if (tmp_right->left_leaf != NULL) {
                                tmp_right = tmp_right->left_leaf;
                            } else {
                                tmp_right->left_leaf = pb->left_leaf;
                                if (pb->father != NULL) {
                                    pb->father->left_leaf = tmp_right;
                                } else {
                                    b->binary_tree_adt = tmp_right;
                                }
                                free(pb);
                                b->number_elements = b->number_elements-1;
                                break;
                            }

                            if (tmp_left->right_leaf != NULL) {
                                tmp_left = tmp_left->right_leaf;
                            } else {
                                tmp_left->right_leaf = pb->right_leaf;
                                if(pb->father != NULL){
                                    pb->father->right_leaf = tmp_left;
                                }else{
                                    b->binary_tree_adt = tmp_right;
                                }
                                free(pb);
                                b->number_elements = b->number_elements-1;
                                break;
                            }
                        }
                    }
                    return 1;
                }
            }
            return 0;
        }
        else
            return 0;
    }

    find.get = get;
    find.remove = remove;

    return find;
}

/**
 *
 * @param this_binary_tree
 * @return
 */
int _empty_binary_tree(BinaryTree *this_binary_tree){
    Stack stack1 = newStack();

    stack1.push(&stack1, this_binary_tree->binary_tree_adt, NULL);
    PBinaryTreeADT currentNode = NULL;
    PBinaryTreeADT prev = NULL;
    while(stack1.size > 0){
        currentNode = (PBinaryTreeADT)stack1.peek(&stack1);
        /* case 1. We are nmoving down the tree. */
        if(prev == NULL || prev->left_leaf == currentNode || prev->right_leaf == currentNode){
            if(currentNode->left_leaf)
                stack1.push(&stack1,currentNode->left_leaf, NULL);
            else if(currentNode->right_leaf)
                stack1.push(&stack1,currentNode->right_leaf, NULL);
            else {
                /* If node is leaf node */
                free(currentNode);
                stack1.pop(&stack1, NULL);
                this_binary_tree->number_elements--;
            }
        }
        /* case 2. We are moving up the tree from left child */
        if(currentNode->left_leaf == prev){
            if(currentNode->right_leaf)
                stack1.push(&stack1,currentNode->right_leaf, NULL);
            else {
                free(currentNode);
                stack1.pop(&stack1, NULL);
                this_binary_tree->number_elements--;
            }
        }
        /* case 3. We are moving up the tree from right child */
        if(currentNode->right_leaf == prev){
            free(currentNode);
            stack1.pop(&stack1, NULL);
            this_binary_tree->number_elements--;
        }
        prev = currentNode;
    }

    destroyStack(&stack1);
    return 1;
}

/**
 *
 * @param this_binary_tree
 * @param callback
 */
ChainingPrint _print_binary_tree(const BinaryTree *this_binary_tree){
    static ChainingPrint methods;//Method chaining
    //DATA STACK
    typedef struct{ PBinaryTreeADT ptb; }struct_ptb;
    static const BinaryTree* b;//copy BinaryTree
    b = this_binary_tree;

    PreorderPrint preorder(void(*callback)(const void* d)) {
        static PreorderPrint preorder;
        static void(*f)(const void* d);
        f = callback;

        //NID: NODO RAIZ, NODO IZQUIERDO, NODO DERECHO
        void left(void){
            if(b->number_elements > 0) {
                static Stack stack1;
                stack1 = newStack();
                stack1.push(&stack1, b->binary_tree_adt, NULL);
                while (stack1.size > 0) {
                    PBinaryTreeADT pbinary_tree_adt1 = (PBinaryTreeADT)stack1.peek(&stack1);
                    stack1.pop(&stack1, NULL);

                    if (f != NULL)
                        f(pbinary_tree_adt1->data);

                    if (pbinary_tree_adt1->right_leaf != NULL) {
                        stack1.push(&stack1, (void *) pbinary_tree_adt1->right_leaf, NULL);
                    }
                    if (pbinary_tree_adt1->left_leaf != NULL) {
                        stack1.push(&stack1, (void *) pbinary_tree_adt1->left_leaf, NULL);
                    }
                }
                destroyStack(&stack1);
            }
        }

        //DIN: NODO DERECHO, NODOD IZQUIERDO, NODOD RAIZ
        void right(void) {
            if (b->number_elements > 0) {
                PBinaryTreeADT currentNode = NULL;
                static Stack stack1;
                stack1 = newStack();
                stack1.push(&stack1, b->binary_tree_adt, NULL);
                PBinaryTreeADT prev = NULL;

                while (stack1.size > 0) {
                    currentNode = (PBinaryTreeADT) stack1.peek(&stack1);
                    /* case 1. We are nmoving down the tree. */
                    if (prev == NULL || prev->left_leaf == currentNode || prev->right_leaf == currentNode) {
                        if (currentNode->right_leaf)
                            stack1.push(&stack1, currentNode->right_leaf, NULL);
                        else if (currentNode->left_leaf)
                            stack1.push(&stack1, currentNode->left_leaf, NULL);
                        else {
                            /* If node is leaf node */
                            if (f != NULL)
                                f(currentNode->data);
                            stack1.pop(&stack1, NULL);
                        }
                    }
                    /* case 2. We are moving up the tree from left child */
                    if (currentNode->right_leaf == prev) {
                        if (currentNode->left_leaf)
                            stack1.push(&stack1, currentNode->left_leaf, NULL);
                        else {
                            if (f != NULL)
                                f(currentNode->data);
                            stack1.pop(&stack1, NULL);
                        }
                    }
                    /* case 3. We are moving up the tree from right child */
                    if (currentNode->left_leaf == prev) {
                        if (f != NULL)
                            f(currentNode->data);
                        stack1.pop(&stack1, NULL);
                    }
                    prev = currentNode;
                }

                destroyStack(&stack1);
            }
        }
        preorder.left = left;
        preorder.right = right;
        return preorder;
    }

    InorderPrint inorder(void(*callback)(const void* d)) {
        static InorderPrint inorder;
        static void(*f)(const void* d);
        f = callback;

        //IND: NODO IZQUIERDO, NODO RAIZ, NODO DERECHO
        void asc(void){
            if(b->number_elements > 0){
                static Stack stack1;
                stack1 = newStack();
                PBinaryTreeADT pbinary_tree_adt1 = b->binary_tree_adt;
                while(stack1.size > 0 || pbinary_tree_adt1 ){
                    if(pbinary_tree_adt1){
                        stack1.push(&stack1, pbinary_tree_adt1, NULL);
                        pbinary_tree_adt1 = (PBinaryTreeADT)pbinary_tree_adt1->left_leaf;
                    }
                    else {
                        pbinary_tree_adt1 = (PBinaryTreeADT)stack1.peek(&stack1);
                        stack1.pop(&stack1, NULL);
                        if(f != NULL)
                            f(pbinary_tree_adt1->data);
                        pbinary_tree_adt1 = (PBinaryTreeADT)pbinary_tree_adt1->right_leaf;
                    }
                }
                destroyStack(&stack1);
            }
        }

        //DNI: NODO DERECHO, NODO RAIZ, NODO IZQUIERDO
        void des(void){
            if(b->number_elements > 0){
                static Stack stack1;
                stack1 = newStack();
                PBinaryTreeADT pbinary_tree_adt1 = b->binary_tree_adt;
                while(stack1.size > 0 || pbinary_tree_adt1 ){
                    if(pbinary_tree_adt1){
                        stack1.push(&stack1, pbinary_tree_adt1, NULL);
                        pbinary_tree_adt1 = (PBinaryTreeADT)pbinary_tree_adt1->right_leaf;
                    }
                    else {
                        pbinary_tree_adt1 = (PBinaryTreeADT)stack1.peek(&stack1);
                        stack1.pop(&stack1, NULL);
                        if(f != NULL)
                            f(pbinary_tree_adt1->data);
                        pbinary_tree_adt1 =(PBinaryTreeADT)pbinary_tree_adt1->left_leaf;
                    }
                }
                destroyStack(&stack1);
            }
        }

        inorder.asc = asc;
        inorder.des = des;
        return inorder;
    }

    PostorderPrint postorder(void(*callback)(const void* d)) {
        static PostorderPrint postorder;
        static void(*f)(const void* d);
        f = callback;

        //IDN: NODO IZQUIERDO, NODO DERECHO, NODO RAIZ
        void left(void){
            if(b->number_elements > 0) {
                PBinaryTreeADT currentNode = NULL;
                static Stack stack1;
                stack1 = newStack();
                stack1.push(&stack1, b->binary_tree_adt, NULL);
                PBinaryTreeADT prev = NULL;

                while (stack1.size > 0) {
                    currentNode = (PBinaryTreeADT) stack1.peek(&stack1);
                    /* case 1. We are nmoving down the tree. */
                    if (prev == NULL || prev->left_leaf == currentNode || prev->right_leaf == currentNode) {
                        if (currentNode->left_leaf)
                            stack1.push(&stack1, currentNode->left_leaf, NULL);
                        else if (currentNode->right_leaf)
                            stack1.push(&stack1, currentNode->right_leaf, NULL);
                        else {
                            /* If node is leaf node */
                            if (f != NULL)
                                f(currentNode->data);
                            stack1.pop(&stack1, NULL);
                        }
                    }
                    /* case 2. We are moving up the tree from left child */
                    if (currentNode->left_leaf == prev) {
                        if (currentNode->right_leaf)
                            stack1.push(&stack1, currentNode->right_leaf, NULL);
                        else {
                            if (f != NULL)
                                f(currentNode->data);
                            stack1.pop(&stack1, NULL);
                        }
                    }
                    /* case 3. We are moving up the tree from right child */
                    if (currentNode->right_leaf == prev) {
                        if (f != NULL)
                            f(currentNode->data);
                        stack1.pop(&stack1, NULL);
                    }
                    prev = currentNode;
                }

                destroyStack(&stack1);
            }
        }

        //NDI: NODO RAIZ, NODO DERECHO, NODO IZQUIERDO
        void right(void){
            if(b->number_elements > 0) {
                static Stack stack1;
                stack1 = newStack();
                stack1.push(&stack1, b->binary_tree_adt, NULL);
                while (stack1.size > 0) {
                    PBinaryTreeADT pbinary_tree_adt1 = (PBinaryTreeADT) stack1.peek(&stack1);
                    stack1.pop(&stack1, NULL);

                    if (f != NULL)
                        f(pbinary_tree_adt1->data);

                    if (pbinary_tree_adt1->left_leaf != NULL) {
                        stack1.push(&stack1, (void *) pbinary_tree_adt1->left_leaf, NULL);
                    }
                    if (pbinary_tree_adt1->right_leaf != NULL) {
                        stack1.push(&stack1, (void *) pbinary_tree_adt1->right_leaf, NULL);
                    }
                }
                destroyStack(&stack1);
            }
        }

        postorder.left = left;
        postorder.right = right;
        return postorder;
    }

    methods.inorder = inorder;
    methods.postorder = postorder;
    methods.preorder = preorder;

    return methods;
}

/*
 *
#include <stdio.h>

typedef struct Tree Tree;

struct Tree
{
  Tree * left, * right;
  int element;
};

Tree *make_empty(Tree *t)
{
  if (t != NULL)
  {
    make_empty(t->left);
    make_empty(t->right);
    free(t);
  }

  return NULL;
}

Tree *find_min(Tree *t)
{
  if (t == NULL)
  {
    return NULL;
  }
  else if (t->left == NULL)
  {
    return t;
  }
  else
  {
    return find_min(t->left);
  }
}

Tree *find_max(Tree *t)
{
  if (t == NULL)
  {
    return NULL;
  }
  else if (t->right == NULL)
  {
    return t;
  }
  else
  {
    return find_max(t->right);
  }
}

Tree *find(int elem, Tree *t)
{
  if (t == NULL)
  {
    return NULL;
  }

  if (elem < t->element)
  {
    return find(elem, t->left);
  }
  else if (elem > t->element)
  {
    return find(elem, t->right);
  }
  else
  {
    return t;
  }
}

//Insert i into the tree t, duplicate will be discarded
//Return a pointer to the resulting tree.
Tree * insert(int value, Tree * t)
{
  Tree * new_node;

  if (t == NULL)
  {
    new_node = (Tree *) malloc (sizeof (Tree));
    if (new_node == NULL)
    {
	    return t;
    }

    new_node->element = value;

	  new_node->left = new_node->right = NULL;
	  return new_node;
  }

  if (value < t->element)
  {
    t->left = insert(value, t->left);
  }
  else if (value > t->element)
  {
	  t->right = insert(value, t->right);
  }
  else
  {
    //duplicate, ignore it
	  return t;
  }
  return t;
}

Tree * delete(int value, Tree * t)
{
  //Deletes node from the tree
  // Return a pointer to the resulting tree
  Tree * x;
  Tree *tmp_cell;

  if (t==NULL) return NULL;

  if (value < t->element)
  {
    t->left = delete(value, t->left);
  }
  else if (value > t->element)
  {
	  t->right = delete(value, t->right);
  }
  else if (t->left && t->right)
  {
    tmp_cell = find_min(t->right);
    t->element = tmp_cell->element;
    t->right = delete(t->element, t->right);
  }
  else
  {
    tmp_cell = t;
    if (t->left == NULL)
      t = t->right;
    else if (t->right == NULL)
      t = t->left;
    free(tmp_cell);
  }

  return t;
}


//printing tree in ascii

typedef struct asciinode_struct asciinode;

struct asciinode_struct
{
  asciinode * left, * right;

  //length of the edge from this node to its children
  int edge_length;

  int height;

  int lablen;

  //-1=I am left, 0=I am root, 1=right
  int parent_dir;

  //max supported unit32 in dec, 10 digits max
  char label[11];
};


#define MAX_HEIGHT 1000
int lprofile[MAX_HEIGHT];
int rprofile[MAX_HEIGHT];
#define INFINITY (1<<20)

//adjust gap between left and right nodes
int gap = 3;

//used for printing next node in the same level,
//this is the x coordinate of the next char printed
int print_next;

int MIN (int X, int Y)
{
  return ((X) < (Y)) ? (X) : (Y);
}

int MAX (int X, int Y)
{
  return ((X) > (Y)) ? (X) : (Y);
}

asciinode * build_ascii_tree_recursive(Tree * t)
{
  asciinode * node;

  if (t == NULL) return NULL;

  node = malloc(sizeof(asciinode));
  node->left = build_ascii_tree_recursive(t->left);
  node->right = build_ascii_tree_recursive(t->right);

  if (node->left != NULL)
  {
    node->left->parent_dir = -1;
  }

  if (node->right != NULL)
  {
    node->right->parent_dir = 1;
  }

  sprintf(node->label, "%d", t->element);
  node->lablen = strlen(node->label);

  return node;
}


//Copy the tree into the ascii node structre
asciinode * build_ascii_tree(Tree * t)
{
  asciinode *node;
  if (t == NULL) return NULL;
  node = build_ascii_tree_recursive(t);
  node->parent_dir = 0;
  return node;
}

//Free all the nodes of the given tree
void free_ascii_tree(asciinode *node)
{
  if (node == NULL) return;
  free_ascii_tree(node->left);
  free_ascii_tree(node->right);
  free(node);
}

//The following function fills in the lprofile array for the given tree.
//It assumes that the center of the label of the root of this tree
//is located at a position (x,y).  It assumes that the edge_length
//fields have been computed for this tree.
void compute_lprofile(asciinode *node, int x, int y)
{
  int i, isleft;
  if (node == NULL) return;
  isleft = (node->parent_dir == -1);
  lprofile[y] = MIN(lprofile[y], x-((node->lablen-isleft)/2));
  if (node->left != NULL)
  {
	  for (i=1; i <= node->edge_length && y+i < MAX_HEIGHT; i++)
    {
	    lprofile[y+i] = MIN(lprofile[y+i], x-i);
    }
  }
  compute_lprofile(node->left, x-node->edge_length-1, y+node->edge_length+1);
  compute_lprofile(node->right, x+node->edge_length+1, y+node->edge_length+1);
}

void compute_rprofile(asciinode *node, int x, int y)
{
  int i, notleft;
  if (node == NULL) return;
  notleft = (node->parent_dir != -1);
  rprofile[y] = MAX(rprofile[y], x+((node->lablen-notleft)/2));
  if (node->right != NULL)
  {
	  for (i=1; i <= node->edge_length && y+i < MAX_HEIGHT; i++)
    {
	    rprofile[y+i] = MAX(rprofile[y+i], x+i);
    }
  }
  compute_rprofile(node->left, x-node->edge_length-1, y+node->edge_length+1);
  compute_rprofile(node->right, x+node->edge_length+1, y+node->edge_length+1);
}

//This function fills in the edge_length and
//height fields of the specified tree
void compute_edge_lengths(asciinode *node)
{
  int h, hmin, i, delta;
  if (node == NULL) return;
  compute_edge_lengths(node->left);
  compute_edge_lengths(node->right);

  // first fill in the edge_length of node
if (node->right == NULL && node->left == NULL)
{
node->edge_length = 0;
}
else
{
if (node->left != NULL)
{
for (i=0; i<node->left->height && i < MAX_HEIGHT; i++)
{
rprofile[i] = -INFINITY;
}
compute_rprofile(node->left, 0, 0);
hmin = node->left->height;
}
else
{
hmin = 0;
}
if (node->right != NULL)
{
for (i=0; i<node->right->height && i < MAX_HEIGHT; i++)
{
lprofile[i] = INFINITY;
}
compute_lprofile(node->right, 0, 0);
hmin = MIN(node->right->height, hmin);
}
else
{
hmin = 0;
}
delta = 4;
for (i=0; i<hmin; i++)
{
delta = MAX(delta, gap + 1 + rprofile[i] - lprofile[i]);
}

//If the node has two children of height 1, then we allow the
//two leaves to be within 1, instead of 2
if (((node->left != NULL && node->left->height == 1) ||
(node->right != NULL && node->right->height == 1))&&delta>4)
{
delta--;
}

node->edge_length = ((delta+1)/2) - 1;
}

//now fill in the height of node
h = 1;
if (node->left != NULL)
{
h = MAX(node->left->height + node->edge_length + 1, h);
}
if (node->right != NULL)
{
h = MAX(node->right->height + node->edge_length + 1, h);
}
node->height = h;
}

//This function prints the given level of the given tree, assuming
//that the node has the given x cordinate.
void print_level(asciinode *node, int x, int level)
{
    int i, isleft;
    if (node == NULL) return;
    isleft = (node->parent_dir == -1);
    if (level == 0)
    {
        for (i=0; i<(x-print_next-((node->lablen-isleft)/2)); i++)
        {
            printf(" ");
        }
        print_next += i;
        printf("%s", node->label);
        print_next += node->lablen;
    }
    else if (node->edge_length >= level)
    {
        if (node->left != NULL)
        {
            for (i=0; i<(x-print_next-(level)); i++)
            {
                printf(" ");
            }
            print_next += i;
            printf("/");
            print_next++;
        }
        if (node->right != NULL)
        {
            for (i=0; i<(x-print_next+(level)); i++)
            {
                printf(" ");
            }
            print_next += i;
            printf("\\");
            print_next++;
        }
    }
    else
    {
        print_level(node->left,
                    x-node->edge_length-1,
                    level-node->edge_length-1);
        print_level(node->right,
                    x+node->edge_length+1,
                    level-node->edge_length-1);
    }
}

//prints ascii tree for given Tree structure
void print_ascii_tree(Tree * t)
{
    asciinode *proot;
    int xmin, i;
    if (t == NULL) return;
    proot = build_ascii_tree(t);
    compute_edge_lengths(proot);
    for (i=0; i<proot->height && i < MAX_HEIGHT; i++)
    {
        lprofile[i] = INFINITY;
    }
    compute_lprofile(proot, 0, 0);
    xmin = 0;
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++)
    {
        xmin = MIN(xmin, lprofile[i]);
    }
    for (i = 0; i < proot->height; i++)
    {
        print_next = 0;
        print_level(proot, -xmin, i);
        printf("\n");
    }
    if (proot->height >= MAX_HEIGHT)
    {
        printf("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
    }
    free_ascii_tree(proot);
}


//driver routine
void main()
{
    //A sample use of these functions.  Start with the empty tree
    //insert some stuff into it, and then delete it
    Tree * root;
    int i;
    root = NULL;

    make_empty(root);

    printf("\nAfter inserting element 10..\n");
    root = insert(10, root);
    print_ascii_tree(root);

    printf("\nAfter inserting element 5..\n");
    root = insert(5, root);
    print_ascii_tree(root);

    printf("\nAfter inserting element 15..\n");
    root = insert(15, root);
    print_ascii_tree(root);

    printf("\nAfter inserting elements 9, 13..\n");
    root = insert(9, root);
    root = insert(13, root);
    print_ascii_tree(root);

    printf("\nAfter inserting elements 2, 6, 12, 14, ..\n");
    root = insert(2, root);
    root = insert(6, root);
    root = insert(12, root);
    root = insert(14, root);
    print_ascii_tree(root);


    printf("\n\nAfter deleting a node (14) with no child..\n");
    root = delete(14, root);
    print_ascii_tree(root);

    printf("\n\nAfter deleting a node (13) with left child..\n");
    root = delete(13, root);
    print_ascii_tree(root);

    printf("\n\nAfter deleting a node (5) with left and right children..\n");
    root = delete(5, root);
    print_ascii_tree(root);

    printf("\n\nAfter deleting a node (10, root node) with left and right children..\n");
    root = delete(10, root);
    print_ascii_tree(root);

    make_empty(root);
}


*/