# Course: CSCI 49900
# Professor: Soumik Dey 
# By: Vishnu Rampersaud

# This file contains the unpruned, post-pruned, pre-pruned, and depth-restricted decision tree algorithms


from collections import OrderedDict, Counter
import operator 
import random
from pprint import pprint
from math import log2
from copy import deepcopy
from copy import copy 

maj = None
minor = None 

# Node class 
# Holds information about a node in the tree 
class Node(): 

    # node class contains information about it value, list of its children, component (whether it's a class(leaf)/branch/node), 
    # list of the values of each class in each of its leaf nodes, and its previous node 
    def __init__(self, x): 
        self.value = x
        self.children = []
        self.component = '' 
        self.classes = []
        self.prev = None

# Decision tree class 
# This class builds a decision tree using an improved/ modified version of the ID3 algorithm 
# Also builds the post-prune and pre-prune versions of the tree. 
class tree(): 

    # private member variables 
    # contains the root of the tree, attributes in the data, and depth of the tree 
    def __init__(self): 
        self.root = None
        self.attributes = {}
        self.curr_depth = 0
        self.data_majority = None
        self.data_minority = None 
        pass

    # return the depth of the tree
    def get_depth(self): 
        return self.curr_depth

    # function to find the majority class of the dataset 
    def __find_majority_class(self, dataset, class_col): 

        global maj 
        global minor

        classes = find_class_values(dataset, class_col)
        if (maj == None and minor == None): 
            if len(set(classes.values())) == 1: 
                print('The amount of classes are equal in this dataset. Which class would you like to set as the majority? ')
                print('Classes are: ')
                for i in classes.keys(): 
                    print(i)
                self.data_majority = input()
                maj = self.data_majority
                for j in classes.keys(): 
                    if j != self.data_majority: 
                        self.data_minority = j
                        minor = self.data_minority
            else: 
                self.data_majority = max(classes, key=classes.get)
                self.data_minority = min(classes, key=classes.get)

        else: 
            self.data_majority = maj
            self.data_minority = minor


    # function to build the decision tree
    # Includes optional parameters for restricting the depth of the tree and implementing a prepruned version of the tree
    # To restrict the depth of the tree, place a value in the depth_stop parameter and the tree will stop at the specified depth 
    # To implement the prepruned version of the tree, these parameters must be set: test_dataset, d_tree, orig_acc, and preprune
    # @ dataset - this is the training dataset that is used to build the tree 
    # @ class_col - the column in the dataset that contains the class values 
    # @ depth_stop - optional parameter in which the depth of the tree is specified; the tree will build until it reaches this depth 
    # @ curr_depth - NOT NEEDED the current depth of the tree; should always start at zero 
    # @ test_dataset - optional parameter, used only for prepruning. This is the validation set for which the tree will be pruned on
    # @ d_tree - optional parameter, used only for prepruning. This should be the current decision tree object, so that we can test the accuracy of the tree as it's being pruned 
    # @ orig_acc - NOT NEEDED optional parameter, used only for prepruning. Contains the current accuracy of the validation set, should start at 0. 
    # @ preprune -  optional parameter, used only for prepruning. When set to 'True', the decision tree will build using the prepruning algorithm
    def buildTree(self, dataset, classes, class_col, depth_stop=None, curr_depth=0, test_dataset=None, d_tree=None, orig_acc=None, preprune=False): 

        # Find all attributes and their values in the dataset, and store them in the class variable 'self.attributes'
        self.__initialize(dataset, class_col)

        # Find the value of each class in the dataset 
        classes = find_class_values(dataset, class_col)

        # Initialize majority class
        self.__find_majority_class(dataset, class_col)

        # Store all attributes in a list to account for all attributes in the current path, and ensure that no attribute is repeated along the path
        unused_attr_list = copy(list(self.attributes.keys()))

        # Find attribute with the highest information gain, and set that as the root node of the tree 
        # attr_split is a list that contains this information about the selected attribute --> [attribute column number, information gain , attribute values]
        attr_split = pick_splitting_attribute(dataset, classes, class_col)

        # If all attributes have an information gain of zero, stop building tree along this path and assign a class (leaf) node; assign the majority class
        # If the class values are equal, then assign the minoritiy class for representational purposes.
        # If all attributes have been used, stop building tree along this path and assign a class (leaf) node; assign the majority class.
        # If the specified depth is reached, stop building tree along this path and assign a class (leaf) node; assign the majority class.
        if curr_depth > self.curr_depth: self.curr_depth = curr_depth
        if attr_split == None or curr_depth == depth_stop or len(unused_attr_list)==0:
            if attr_split == None: 
                if len(set(classes.values())) == 1: 
                    majority_class = self.data_minority
                else: 
                    majority_class = max(classes, key=classes.get)
            else: 
                if len(set(classes.values())) == 1: 
                    majority_class = self.data_majority
                else: 
                    majority_class = max(classes, key=classes.get)
            self.root = Node(majority_class) 
            self.root.component = 'class:'
            self.root.classes.append(deepcopy(classes))
            return 

        # Pre-pruning algorithm 
        if preprune:
                
            # Find and place majority class under branch
            if len(set(classes.values())) == 1: 
                pre_majority_class = self.data_majority
            else: 
                pre_majority_class = max(classes, key=classes.get)

            self.root = Node(pre_majority_class) 
            self.root.component = 'class:'
            class_amount = deepcopy(classes)
            self.root.classes.append(class_amount)

            # test accuracy of tree now 
            preprune_acc = accuracy(test_dataset, class_col, d_tree)
            #print('orig_acc', orig_acc, 'preprune acc:', preprune_acc)

            # if acc increases, then remove the class node that was just placed and continue to split on the attribute 
            if preprune_acc >= orig_acc[0]:
                orig_acc[0] = preprune_acc 
                self.root.classes.remove(class_amount)
                self.root = None

            else: 
                return

        # Set selected attribute with the highest information gain as the root 
        attr_col_name = 'Attr_' + str(attr_split[0])
        self.root = Node(attr_col_name)
        self.root.prev = self.root
        self.root.prev.prev = self.root

        # remove attribute from the list of unused attributes in this path 
        unused_attr_list.remove(attr_col_name)

        # Get all sorted attribute values of the current node 
        attr_values = OrderedDict(sorted(attr_split[2].items()))

        # use each attribute value as a branch for the attribute node; expand along each branch 
        for k, branch in enumerate(self.attributes[attr_col_name]): 
            
            # create a new branch from the attribute node using the current attribute value
            new_branch = Node(branch) 
            new_branch.component = 'branch:'
            new_branch.prev = self.root
            self.root.children.append(new_branch)

            # if the branch (attribute value) is not part of this dataset, then set it to the majority class and move to the next branch 
            # In original ID3 algorithm, these branches are usually set to null
            if branch not in list(attr_values.keys()): 
                if len(set(classes.values())) == 1: 
                    majority_class = self.data_majority
                else: 
                    majority_class = max(classes, key=classes.get)
                null_class = Node(majority_class)
                null_class.component = 'class:'
                null_class.prev = self.root
                self.root.children[k].children.append(null_class)
                continue

            # Split dataset based on the branch (attribute value)
            new_dataset = split_dataset(branch, attr_split[0], dataset)
           
            # Keep expanding along this branch if possible 
            if len(new_dataset) !=0:
                self.__buildTree(new_dataset, class_col, self.root.children[k], depth_stop, curr_depth+1, test_dataset, d_tree, orig_acc, preprune, unused_attr_list)

            # If there is no more data, then set a class (leaf) node as the majority class 
            else: 
                if len(set(classes.values())) == 1: 
                    m_class = self.data_majority
                else: 
                    m_class = max(classes, key=classes.get)

                new_leaf = Node(m_class) 
                new_leaf.component = 'class:'
                new_leaf.prev = self.root.children[k]
                class_amount = deepcopy(classes)
                self.root.classes.append(class_amount)
                #print(new_classes)
                self.root.children[k].children.append(new_leaf)

    # Internal function to build tree 
    # Only a necessity because python doesn't allow a NoneType value to be changed recursively
    # takes all parameters from buildTree() function 
    # @ dataset - this is the training dataset that is used to build the tree 
    # @ class_col - the column in the dataset that contains the class values 
    # @ depth_stop - optional parameter in which the depth of the tree is specified; the tree will build until it reaches this depth 
    # @ curr_depth - NOT NEEDED the current depth of the tree; should always start at zero 
    # @ test_dataset - optional parameter, used only for prepruning. This is the validation set for which the tree will be pruned on
    # @ d_tree - optional parameter, used only for prepruning. This should be the current decision tree object, so that we can test the accuracy of the tree as it's being pruned 
    # @ orig_acc - NOT NEEDED optional parameter, used only for prepruning. Contains the current accuracy of the validation set, should start at 0. 
    # @ preprune -  optional parameter, used only for prepruning. When set to 'True', the decision tree will build using the prepruning algorithm
    # @ unused_attr_list - keeps track of the attributes used in the current path 
    def __buildTree(self, dataset, class_col, curr_node, depth_stop, curr_depth, test_dataset, d_tree, orig_acc, preprune, unused_attr_list): 

        # Store all attributes in a list to account for all attributes in the current path, and ensure that no attribute is repeated along the path
        curr_unused_attr_list = copy(unused_attr_list)

        # Find the value of each class in the dataset 
        classes = find_class_values(dataset, class_col)
        if self.data_majority not in classes: 
            classes[self.data_majority] = 0
        elif self.data_minority not in classes: 
            classes[self.data_minority] = 0

        # Find attribute with the highest information gain, and set that as the root node of the tree 
        # attr_split is a list that contains this information about the selected attribute --> [attribute column number, information gain , attribute values]
        attr_split = pick_splitting_attribute(dataset, classes, class_col)

        # If all attributes have an information gain of zero, stop building tree along this path and assign a class (leaf) node; assign the majority class
        # If the class values are equal, then assign the minoritiy class for representational purposes.
        # If all attributes have been used, stop building tree along this path and assign a class (leaf) node; assign the majority class.
        # If the specified depth is reached, stop building tree along this path and assign a class (leaf) node; assign the majority class.
        if curr_depth > self.curr_depth: self.curr_depth = curr_depth
        if attr_split == None or curr_depth == depth_stop or len(curr_unused_attr_list)==0:
            if attr_split == None: 
                if len(set(classes.values())) == 1: 
                    majority_class = self.data_minority
                else: 
                    majority_class = max(classes, key=classes.get)
            else: 
                if len(set(classes.values())) == 1: 
                    majority_class = self.data_majority
                else: 
                    majority_class = max(classes, key=classes.get)
            class_branch = Node(majority_class) 
            class_branch.component = 'class:'
            class_branch.prev = curr_node
            curr_node.prev.classes.append(deepcopy(classes))
            curr_node.children.append(class_branch)
            return 

        # PREPRUNE ALGORITHM 
        if preprune: 

            # Find and place majority class under branch
            if len(set(classes.values())) == 1: 
                pre_majority_class = self.data_majority
            else: 
                pre_majority_class = max(classes, key=classes.get)
    
            new_leaf = Node(pre_majority_class) 
            new_leaf.component = 'class:'
            new_leaf.prev = curr_node
            class_amount = deepcopy(classes)
            curr_node.children.append(new_leaf)
            curr_node.prev.classes.append(class_amount)

            # test accuracy of tree now 
            preprune_acc = accuracy(test_dataset, class_col, d_tree)
            #print('orig_acc', orig_acc, 'preprune acc:', preprune_acc)

            # if acc increases, then remove the class node that was just placed and continue to split on the attribute 
            if preprune_acc >= orig_acc[0]:
                orig_acc[0] = preprune_acc 
                curr_node.prev.classes.remove(class_amount)
                curr_node.children.remove(new_leaf)
                del new_leaf

            else: 
                return 

        # Set selected attribute with the highest information gain as a node on this path
        attr_col_name = 'Attr_' + str(attr_split[0])
        curr_node.children.append(Node(attr_col_name))
        index = len(curr_node.children) - 1
        curr_node.children[index].prev = curr_node

        # remove attribute from the list of unused attributes in this path 
        curr_unused_attr_list.remove(attr_col_name)

        # Get all sorted attribute values of the current node 
        attr_values = OrderedDict(sorted(attr_split[2].items()))

         # use each attribute value as a branch for the attribute node; expand along each branch 
        for k, branch in enumerate(self.attributes[attr_col_name]): 
            
            # create a new branch from the attribute node using the current attribute value
            new_branch = Node(branch) 
            new_branch.component = 'branch:'
            new_branch.prev = curr_node.children[index]
            curr_node.children[index].children.append(new_branch)

            # if the branch (attribute value) is not part of this dataset, then set it to the majority class and move to the next branch 
            # In original ID3 algorithm, these branches are usually set to null
            if branch not in list(attr_values.keys()): 
                if len(set(classes.values())) == 1: 
                    majority_class = self.data_majority
                else: 
                    majority_class = max(classes, key=classes.get)
                null_branch = Node(majority_class)
                null_branch.component = 'class:'
                null_branch.prev = curr_node.children[index]
                curr_node.children[index].children[k].children.append(null_branch)
                continue

            # Split dataset based on the branch (attribute value)
            new_dataset = split_dataset(branch, attr_split[0], dataset)

             # Keep expanding along this branch if possible 
            if len(new_dataset) !=0:
                self.__buildTree(new_dataset, class_col, curr_node.children[index].children[k], depth_stop, curr_depth+1, test_dataset, d_tree, orig_acc, preprune, curr_unused_attr_list)
            # If there is no more data, then set a class (leaf) node as the majority class 
            else: 
                if len(set(classes.values())) == 1: 
                    m_class = self.data_majority
                else: 
                    m_class = max(classes, key=classes.get)

                new_leaf = Node(m_class) 
                new_leaf.component = 'class:'
                new_leaf.prev = curr_node.children[index].children[k]
                class_amount = deepcopy(classes)
                curr_node.children[index].classes.append(class_amount)
                curr_node.children[index].children[k].children.append(new_leaf)

    # function to classify a row of data using the built tree 
    # @ row - row of data to be classified 
    def contains(self, row): 
        return self.__contains(row, self.root)

    # internal function to classify a row of data using the built tree 
    # @ row - row of data to be classified 
    # @ curr_node - the current node of the tree 
    def __contains(self, row, curr_node): 
        
        if len(curr_node.children) > 0: 

            # get the current attribute from current node 
            curr_attr = int(curr_node.value[-1])

            # get the value of that attribute from test row 
            test_attr_value = row[curr_attr]

            # move down the branch of the tree that has the attribute value of the test row
            attr_index = None 
            for i, child in enumerate(curr_node.children): 
                if child.value == test_attr_value: 
                    attr_index = i
                    break 

            # in case attribute value is not in the tree, return no predicted classification 
            if attr_index == None: 
                return attr_index

            # if this node is not a class (leaf) then keep traversing 
            next_node = curr_node.children[attr_index].children[0]
            return self.__contains(row, next_node)

        else: 
            # if a class (leaf) node is reached, then return the predicted class
            return curr_node.value

    # function to collect all attributes and their values from the dataset 
    def __initialize(self, dataset, class_col): 

        # Find the number of attributes in the dataset 
        num_cols = len(dataset[0])

        # Store all attributes into a dictionary 
        for i in range (num_cols): 
            if i == class_col: 
                continue
            self.attributes[('Attr_'+str(i))] = []

        # Store all attribute values into dictionary with its corresponding attribute
        for attr, branch in self.attributes.items(): 
            attr_col = int(attr[-1])
            for row in dataset: 
                if row [attr_col] not in branch: 
                    branch.append(row[attr_col])
            branch.sort()
        return 

    # Post-pruning function 
    # Prunes the tree after it is built 
    # This reduces overfitting and should help increase the accuracy of the tree
    # @ test_dataset - this is the validation set that will be used to prune the tree
    # @ class_col - this is the column number that contains the class values 
    # @ d_tree - This should be the current decision tree object that is going to be pruned 
    # @ orig_acc - 
    def post_pruning(self, test_dataset, class_col, d_tree, orig_acc): 
        self.__post_pruning(self.root, test_dataset, class_col, d_tree, orig_acc)

    # Internal function for post pruning 
    # @ curr_node - current node of the tree 
    # @ test_dataset - this is the validation set that will be used to prune the tree
    # @ class_col - this is the column number that contains the class values 
    # @ d_tree - This should be the current decision tree object that is going to be pruned 
    # @ orig_acc -
    def __post_pruning(self, curr_node, test_dataset, class_col, d_tree, orig_acc): 

        # traverse the tree in a bottom-up fashion 
        for child in curr_node.children: 
            if child.children: 
                self.__post_pruning(child, test_dataset, class_col, d_tree, orig_acc)

        # determine if the current node is the last attribute node in a path 
        leaf = True 
        if curr_node.value.startswith('Attr_'): 
            for c in curr_node.children:
                if leaf == False: break
                for d in c.children: 
                    if d.component != 'class:': 
                        leaf = False
                        break
        else: 
            leaf = False

        # When at the last attribute node in the path, prune that node and check its accuracy 
        if leaf: 

            # computes the total values of each class(leaf) node and combines them
            # this will be the class values for the pruned node 
            temp_dict = {}
            for i in range ( len(curr_node.classes) ): 
                temp_dict = Counter(temp_dict) + Counter(curr_node.classes[i])

            # Find the majority class of this set of computed values
            if len(set(temp_dict.values())) == 1: 
                majority_class = self.data_majority
            else: 
                majority_class = max(temp_dict, key=temp_dict.get)
            
            # store information of current attribute node before pruning
            temp_values = {'value':curr_node.value,
                           'children':copy(curr_node.children),
                           'component':curr_node.component, 
                           'classes':copy(curr_node.classes),
                           'prev_node_classes':copy(curr_node.prev.prev.classes)
                           }

            # Remove/ prune node 
            curr_node.value = majority_class
            curr_node.children = []
            curr_node.component = 'class:'
            curr_node.classes = []
            curr_node.prev.prev.classes.append(temp_dict)

            # test the accuracy of the tree after pruning the node 
            acc = accuracy(test_dataset, class_col, d_tree)

            # if the accuracy decreases, then replace the pruned node with its original information
            # if the accuracy increases or remains the same, then keep the current node pruned 
            if acc < orig_acc[0]: 
                # Replace node 
                curr_node.value = temp_values['value']
                curr_node.children = copy(temp_values['children'])
                curr_node.component = temp_values['component']
                curr_node.classes = copy(temp_values['classes'])
                curr_node.prev.prev.classes = copy(temp_values['prev_node_classes'])
            else: 
                # update current accuracy of the tree 
                orig_acc[0] = acc

    # function to print the decision tree 
    # prints the tree using indentations to make it easier to visualize 
    def printTree(self): 
        self.__printTree(self.root)

    # Internal function to print tree 
    # @ curr_node - the current node of the tree
    # @ depth - the current depth of the node 
    # @ indent - amount of spaces needed for the current node 
    def __printTree(self, curr_node, depth=0, indent=4): 
        if curr_node != None:
            print("{}{}{}{}".format(" "*(indent*depth), curr_node.component, curr_node.value, curr_node.classes))
            for i in range ( len(curr_node.children)): 
                self.__printTree(curr_node.children[i], depth=depth+1, indent=indent)

    # function to write the decision tree to a file 
    # writes using indentations to make it easier to visualize 
    def writeTree(self): 
        self.__writeTree(self.root)

    # Internal function to write decision tree to file 
    def __writeTree(self, curr_node, depth=0, indent=4): 
        if curr_node != None:
            file.write("{}{}{}{}".format(" "*(indent*depth), curr_node.component, curr_node.value, '\n'))
            for i in range ( len(curr_node.children)): 
                self.__writeTree(curr_node.children[i], depth=depth+1, indent=indent)

    pass

# function to load file dataset 
# returns a list containing the dataset
# parses file accordingly 
def loadFile(filename): 

    # create new list 
    dataset = []

    # open the data file 
    file = open(filename, 'r')

    # load each row of data into list
    # each value is separated by a space character 
    # removes the last element in the row as it is not needed in this dataset
    for line in file: 
        if not line: continue
        line = line.strip()
        data_row = line.split(" ")
        data_row = data_row[:-1]
        dataset.append(data_row)

    return dataset

# Function to find the amount of each class in a given attribute 
# @ dataset - data that is used 
# @ attr_col - attribute column that is selected from the dataset to extract the class count
# @ class_col - column number that corresponds to the class column 
def find_attr_class_count(dataset, classes, attr_col, class_col): 

    attr_values = OrderedDict()
    for row in dataset: 
        if row[attr_col] not in attr_values: 
            for i, key in enumerate(classes): 
                if row[class_col] == key and i == 0: 
                    attr_values[row[attr_col]] = [1, 0]
                elif row[class_col] == key and i == 1: 
                    attr_values[row[attr_col]] = [0, 1]
        elif row[attr_col] in attr_values: 
            for i, key in enumerate(classes): 
                if row[class_col] == key and i == 0: 
                    attr_values[row[attr_col]][i] += 1
                elif row[class_col] == key and i == 1: 
                    attr_values[row[attr_col]][i] += 1
    return attr_values

# Function to find the amount of each class in a given dataset 
# @ dataset - dataset that is used to extract the class count 
# @ class_col - column number that corresponds to the class column 
def find_class_values(dataset, class_col): 
    #print(class_col)
    classes = OrderedDict()
    for row in dataset: 
        if row[class_col] not in classes: 
            classes[row[class_col]] = 1
        else: 
            classes[row[class_col]] += 1
    return classes

# function to find entropy 
def find_entropy(class1, class2, total): 

    if (class1 == 0 and class2 != 0): 
        entropy = ( 0 - ( (class2/total) * log2(class2/total) ) )
    elif (class1 != 0 and class2 == 0): 
        entropy = ( (-class1/total) * log2(class1/total) )
    else: 
        entropy = ( (-class1/total) * log2(class1/total) ) - ( (class2/total) * log2(class2/total) )

    return entropy 

# function to find information gain 
def find_information_gain(classes, attributes): 

    # store each class and its amount into a variable 
    total = 0
    class1 = None 
    class2 = None 
    for i, key in enumerate(classes): 
        if i == 0: 
            class1 = classes[key]
        elif i ==1: 
            class2 = classes[key]
        total += classes[key]

    entropy_S = find_entropy(class1, class2, total)

    summation = 0
    for key in attributes: 
        pos = attributes[key][0]
        neg = attributes[key][1]
        attr_total = pos + neg
        attr_entropy = find_entropy(pos, neg, attr_total)
        summation += ( attr_total/total ) * ( attr_entropy )
    information_gain = entropy_S - summation

    return information_gain

# function to pick the attribute with highest information gain while building the tree 
# @ dataset - data that is used to select the correct attribute 
# @ classes - the current class count of dataset 
# @ class_col - the column number in the dataset that corresponds to the class column 
def pick_splitting_attribute(dataset, classes, class_col): 

    #number of columns 
    num_of_cols = len( dataset[0] )

    #list to hold the information gain of each attribute
    info_gain = []

    #find information gain of all attributes 
    for attr_col in range (num_of_cols): 
        
        # skip over the class column 
        if attr_col == class_col or (class_col == -1 and attr_col == num_of_cols-1): 
            continue

        #find number of classes for each attribute value in specified attribute
        attr_values = find_attr_class_count(dataset, classes, attr_col, class_col)
        
        # find information gain of specified attribute
        attr_gain = find_information_gain(classes, attr_values)

        #add information gain of this attribute to list 
        info_gain.append([attr_col, attr_gain, attr_values])

    return find_max(info_gain)

# function to find the attribute with the maximum information gain 
# @ x - list of information gains and their corresponding attributes 
def find_max(x): 
    max = 0
    for i in range ( len(x) ): 
        
        if x[i][1] > max: 
            max = x[i][1]
            max_attr = x[i]

    # if all attribute information gains are zero, return NoneType 
    if max == 0: 
        max_attr = None 

    return max_attr

# function to split dataset based on the branch (attribute column and value)
# @ attr_value - the value of the attribute  
# @ attr_col - the column number of the attribute 
# @ dataset - data that will be split according to attribute value 
def split_dataset(attr_value, attr_col, dataset): 

    new_dataset = []

    for i in range (0, len(dataset)): 
        if dataset[i][attr_col] == attr_value: 
            new_dataset.append(dataset[i])

    return new_dataset

# function to test accuracy of decision tree
def accuracy (test_dataset, class_col, tree):

    tp=0 
    tn=0
    fp=0
    fn=0

    # check
    sum = 0
    for row in test_dataset: 

        actual = row[class_col]
        prediction = tree.contains(row)
        
        if prediction == actual: 
            sum += 1
            if prediction=='1': 
                tp += 1
            elif prediction == '0': 
                tn += 1
        else: 
            if prediction == '1': 
                fp += 1
            elif prediction == '0': 
                fn += 1


    #print('true positive',tp)
    #print('true negative',tn)
    #print('false positive',fp)
    #print('false negative',fn)
    acc = sum / (len(test_dataset))

    return acc

if __name__ == '__main__': 

    # Ask user for train file (we will split this into train and val files using an 80:20 split) 
    #print('Please enter the monks file you want to test:')
    #filename = input()

    # Create a results file to store the various tree outputs
    file = open('./FAT/results.txt', 'w')

    # Create a stratified training file using the inputted dataset 
    train_file = open('./FAT/stratified-train.txt', 'w')

    # Create a stratified validation file using the inputted dataset 
    val_file = open('./FAT/stratified-validation.txt', 'w')

    # load in inputted train and test file
    full_train_dataset = loadFile("./FAT/train monks.txt")
    test_dataset = loadFile("./FAT/test monks.txt")

    # initialized lists to store the stratified train and val data 
    train_dataset = []
    val_dataset = []
   
    #specify classification column
    class_col = 0

    #----------------------------------STRATIFICATION BEGINS----------------------------------#

    # if you want to randomize data before stratification, uncomment the line of code below:
    #random.shuffle(full_train_dataset)

    #find total amount of each class of full input data --> stored into a dictionary (only two classes) 
    strat_classes = find_class_values(full_train_dataset, class_col)

    # amount of training rows needed in total for 80:20 split
    total_train_rows = round (len(full_train_dataset) * 0.8) 

    # amount of validation rows needed in total for 80:20 split
    total_val_rows = round (len(full_train_dataset) * 0.2)

    # amount of rows needed in train and val for each class 
    train_amount = {}
    val_amount = {}
    for key, item in strat_classes.items(): 
        train_amount[key] = round(item * 0.8)
        val_amount[key] = round(item * 0.2)

    # Store training data into list with stratified proportions 
    for key, item in train_amount.items():
        j = 0
        for row in full_train_dataset[:]:
            if j >= item: 
                #print('train set for class: {} needs {} items'.format(key,item))
                break
            if row[class_col] == key: 
                j+=1
                train_dataset.append(row)
                full_train_dataset.remove(row)

    # Store validation data into list with stratified proportions
    for key, item in val_amount.items():
        j = 0
        for row in full_train_dataset[:]:
            if j >= item: 
                #print('train set for class: {} needs {} items'.format(key,item))
                break
            if row[class_col] == key: 
                j+=1
                val_dataset.append(row)
                full_train_dataset.remove(row)

    # store train data into output file 
    for row in train_dataset: 
        for element in row: 
            train_file.write('{},'.format(element))
        train_file.write('\n')
    train_file.close()

    # store validation data into output file
    for row in val_dataset: 
        for element in row: 
            val_file.write('{},'.format(element))
        val_file.write('\n')
    val_file.close()

    #----------------------------------STRATIFICATION ENDS------------------------------------#

    del full_train_dataset
    full_train_dataset = loadFile("./FAT/train monks.txt")
    #find total amount of each class of training data --> stored into a dictionary (only two classes) 
    classes = find_class_values(full_train_dataset, class_col)
    
    # Build basic decision tree 
    decision_tree = tree()
    decision_tree.buildTree(full_train_dataset, classes, class_col)
    

    # get height of decision tree 
    dec_tree_height = decision_tree.get_depth() + 1
  
    # restrict height of decision tree until fully built; check accuracy on each height 
    for i in range(dec_tree_height): 
        depth_decision_tree = tree()
        depth_decision_tree.buildTree(full_train_dataset, classes, class_col, depth_stop=i)
        depth_dec_tree_acc = accuracy(test_dataset, class_col, depth_decision_tree)
        print('Accuracy of decision tree on depth = {}: {}'.format(i, depth_dec_tree_acc))
        file.write('Decision tree on depth = {}: \n\n'.format(i))
        depth_decision_tree.writeTree()
        del depth_decision_tree

    # Write basic unpruned decision tree to file 
    file.write('Basic decision tree: \n\n')
    decision_tree.writeTree()

    # test accuracy of the basic decision tree on the test set 
    basic_dec_tree_acc = accuracy(test_dataset, class_col, decision_tree)
    print('\nAccuracy of decision tree on test set without pruning:', basic_dec_tree_acc)

    
    # post prune tree
    del classes 
    classes = find_class_values(train_dataset, class_col)

    decision_tree_post = tree()
    decision_tree_post.buildTree(train_dataset, classes, class_col)
    #decision_tree_post.printTree()

    post_dec_tree_acc = accuracy(test_dataset, class_col, decision_tree_post)
    print('\nAccuracy of stratified decision tree on test set without pruning:', post_dec_tree_acc)

    # test accuracy of basic decision tree on validation set 
    val_acc = []
    val_acc.append (accuracy(val_dataset, class_col, decision_tree_post) )
    print('Accuracy of decision tree on validation set without pruning:', val_acc[0])

    # Prune decision tree using post-pruning 
    decision_tree_post.post_pruning(val_dataset, class_col, decision_tree_post, val_acc)
    file.write('\n\nPost-pruned decision tree: \n\n')
    decision_tree_post.writeTree()

    # validation accuracy of decision tree after post pruning 
    val_acc_post = accuracy(val_dataset, class_col, decision_tree_post)
    print('Accuracy of decision tree on validation set after post-pruning:', val_acc_post)

    # test accuracy of decision tree after post pruning 
    test_acc = accuracy(test_dataset, class_col, decision_tree_post)
    print('Accuracy of decision tree on test set after post-pruning:', test_acc)

    # Build decision tree using pre-pruning technnique 
    preprune = True
    preprune_acc = [0]
    preprune_tree = tree() 
    preprune_tree.buildTree(train_dataset, classes, class_col, test_dataset=val_dataset, d_tree=preprune_tree, orig_acc=preprune_acc, preprune=preprune)
    file.write('\n\nPre-pruned decision tree: \n\n')
    preprune_tree.writeTree()

    # test accuracy of pre-pruned decision tree
    pre_acc = accuracy(test_dataset, class_col, preprune_tree)
    print('Accuracy of decision tree on test set after pre-pruning:', pre_acc)
    
    
    file.close()
    pass
