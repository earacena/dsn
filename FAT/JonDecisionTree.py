#Jonathan Balina
import math
import csv
import random


def normalize(df):
    for i in range(0, len(df)):
        for j in range(1, 7):
            df[i][j] = (df[i][j] - 1) / 5

    return df


def load(file):
    with open(file) as csvfile:
        csvReader = csv.reader(csvfile, delimiter=" ")
        data = []
        for row in csvReader:

            row.pop()
            row.pop(0)
            # print(x)
            d = []

            for r in row:
                d.append(int(r))
            data.append(d)
        return data


def read_csv(file):
    with open(file) as csvfile:
        csvReader = csv.reader(csvfile)
        data = []
        for row in csvReader:
            row.pop()
            row.pop(0)
            # print(x)
            d = []
            for r in row:
                d.append(int(r))
            data.append(d)
        return data


header = ["class", "a1", "a2", "a3", "a4", "a5", "a6", "a7"]


def unique_items(data, col):
    return set([item[col] for item in data])


def num_classes(data):
    classes = {}
    for item in data:
        class_item = item[0]
        if class_item not in classes:
            classes[class_item] = 0
        classes[class_item] += 1
    return classes


def majority_class(data):
    max_key = 0
    max_count = 0
    for key in num_classes(data):
        if num_classes(data)[key] > max_count:
            max_key = key
            max_count = num_classes(data)[key]
    return max_key


class Comparison:
    def __init__(self, col):
        self.col = col


def split(data, col):
    split_data = {}
    values = set([item[col] for item in data])
    for val in values:
        classcount = {}
        new_data = []
        class1 = 0
        class2 = 0
        for item in data:
            if item[col] == val:
                new_data.append(item)
                if item[0] == 0:
                    class1 += 1
                elif item[0] == 1:
                    class2 += 1
        classcount["data"] = new_data
        classcount["class1"] = class1
        classcount["class2"] = class2
        split_data[val] = classcount
    return split_data


"""
def gini(data):
    class_count = num_classes(data)
    impurity = 1
    for class_item in class_count:
        class_prob = class_count[class_item] / float(len(data))
        impurity -= class_prob**2
    return impurity
"""


def calc_entropy_data(dic, data):
    #class1_count = 0
    #class2_count = 0
    entropy = 0
    for key in dic:
        #class1_count += dic[key]["class1"]
        #class2_count += dic[key]["class2"]
        #print("class counts:")
        #print(dic[key]["class1"])
        #print(dic[key]["class2"])
        entropy += (dic[key]["class1"] + dic[key]["class2"]) / len(data) * (calc_entropy(dic[key]["class1"], dic[key]["class2"]))
    return entropy


def calc_entropy(class1_count, class2_count):
    p = 0
    if not class1_count == 0 and not class2_count == 0:
        p = -(class1_count/(class1_count+class2_count))*math.log(class1_count/(class1_count+class2_count), 2)\
            - (class2_count/(class1_count+class2_count))*math.log(class2_count/(class1_count+class2_count), 2)
    return p
"""
    p = float(len(left)) / (len(left) + len(right))
    return uncertainty - p * gini(left) - (1 - p) * gini(right)
"""


def best_split(data, no_pass=[]):
    #pass_col = False
    high_gain = 0
    best_col = None
    entropy = 0
    features = len(data[0])
    for col in range(1, features):
        if col in no_pass:
            continue
        #print(data)

        gain = 0
        classcount = split(data, col)
        #print(classcount)

        entropy = calc_entropy_data(classcount, data)
        #print(col)
        #print(entropy)
        class1_count, class2_count = 0, 0
        if 0 in num_classes(data):
            class1_count = num_classes(data)[0]
        if 1 in num_classes(data):
            class2_count = num_classes(data)[1]
            #print(entropy)
        gain = calc_entropy(class1_count, class2_count) - entropy
        #print(gain)
        if gain > high_gain:
            high_gain, best_col = gain, col
    #print(best_col)
    #print(high_gain)
    return high_gain, best_col


class Leaf:

    def __init__(self, dic):
        self.results = dic

    @classmethod
    def from_items(self, data):
        return Leaf(num_classes(data))


class DecisionNode:
    def __init__(self, col, data=[]):
        self.col = col
        self.children = {}
        dic = {}
        dic[majority_class(data)] = len(data)
        self.majority = dic

    @classmethod
    def from_dnode(self, node):
        self.col = node.col
        self.children = node.children
        self.majority = node.majority



def build_tree(data, orig_data=[], no_pass=[]):
    if len(orig_data) == 0:
        orig_data = data
    #print(data)
    gain, col = best_split(data, no_pass)

    if gain == 0:
        if len(num_classes(data)) != 1:
            dic = {}
            dic[majority_class(data)] = len(data)
            return Leaf(dic)
        else:
            return Leaf.from_items(data)
    node = DecisionNode(col, data)
    split_data = split(data, col)
    for key in unique_items(orig_data, col):
        if key not in split_data:#len(split_data[key]["data"]) == 0:
            dic = {}
            dic[majority_class(data)] = 0
            node.children[key] = Leaf(dic)
        else:
            node.children[key] = build_tree(split_data[key]["data"], orig_data, no_pass)
    #for key in split_data:
    #    node.children[key] = build_tree(split_data[key]["data"], no_pass)

    return node


def preprune_tree(data, valdata, top_node=None, orig_data=[], no_pass=[]):
    first_node = False
    if top_node == None:
        first_node = True
    if len(orig_data) == 0:
        orig_data = data
    #print(data)
    gain, col = best_split(data, no_pass)
    #print("hey")
    #print(gain)
    #no_pass.append(col)
    #print(col)
    if gain == 0:
        if len(num_classes(data)) != 1:
            dic = {}
            dic[majority_class(data)] = len(data)
            return Leaf(dic)
        else:
            return Leaf.from_items(data)
    node = DecisionNode(col, data)
    prune_acc = 0
    if first_node:
        top_node = node
        """
    else:
        dic = {}
        dic[majority_class(data)] = len(data)
        node = Leaf(dic)
        prune_acc = ret_accuracy(valdata, top_node)
        node = DecisionNode(col, data)
"""
    split_data = split(data, col)
    val_split = split(valdata, col)
    for key in unique_items(orig_data, col):
        if key not in split_data:#len(split_data[key]["data"]) == 0:
            dic = {}
            dic[majority_class(data)] = 0
            node.children[key] = Leaf(dic)
        else:
            if key not in val_split:
                dic = {"data": [], "class1": 0, "class2": 0}
                val_split[key] = dic
            node.children[key] = preprune_tree(split_data[key]["data"], val_split[key]["data"], top_node, orig_data, no_pass)
    if not first_node:
        if ret_accuracy(valdata, Leaf(dic={majority_class(data):len(data)})) < ret_accuracy(valdata, node):
            return Leaf(dic={majority_class(valdata):len(data)})
        """
    if not first_node:
        temp = node
        dec_acc = ret_accuracy(valdata, top_node)
        dic = {}
        dic[majority_class(data)] = len(data)
        node = Leaf(dic)
        prune_acc = ret_accuracy(valdata, top_node)

        if prune_acc < dec_acc:
            node = temp
        else:
            return node
            """
    #for key in split_data:
    #    node.children[key] = build_tree(split_data[key]["data"], no_pass)

    return node


def depth_prune_tree(data, depth, orig_data=[], no_pass=[]):
    if len(orig_data) == 0:
        orig_data = data
    #print(data)
    if depth <= 0:
        dic = {}
        dic[majority_class(data)] = len(data)
        return Leaf(dic)
    gain, col = best_split(data, no_pass)
    #print("hey")
    #print(gain)
    #no_pass.append(col)
    #print(col)
    if gain == 0:
        if len(num_classes(data)) != 1:
            dic = {}
            dic[majority_class(data)] = len(data)
            return Leaf(dic)
        else:
            return Leaf.from_items(data)
    node = DecisionNode(col, data)
    split_data = split(data, col)
    for key in unique_items(orig_data, col):
        if key not in split_data:#len(split_data[key]["data"]) == 0:
            dic = {}
            dic[majority_class(data)] = 0
            node.children[key] = Leaf(dic)
        else:
            node.children[key] = depth_prune_tree(split_data[key]["data"], depth-1, orig_data, no_pass)
    #for key in split_data:
    #    node.children[key] = build_tree(split_data[key]["data"], no_pass)

    return node

def print_tree(node, spacing=""):
    if isinstance(node, Leaf):
        print(spacing + "Predict", node.results)
        return

    print(spacing + "col:" + str(node.col))
    for key in node.children:

        print(spacing + str(key))
        print_tree(node.children[key], spacing + "  ")


def all_leaves(node):
    if isinstance(node, DecisionNode):
        for key in node.children:
            if not isinstance(node.children[key], Leaf):
                return False
        return True
    return False


def turn_leaf(node, data): #data is val set
    initial_wrong_count = len(data)
    for row in data:
        if (len(classify(row, node)) == 1) and (classify(row, node).get(row[0]) is not None):
            initial_wrong_count -= 1
    new_dict = {}
    turn_l = False
    for key in num_classes(data):
        eCount = len(data) - num_classes(data)[key]
        if eCount <= initial_wrong_count:
            initial_wrong_count = eCount
            turn_l = True
            new_dict.clear()
            new_dict[key] = len(data)
    return turn_l, new_dict


def prune(node, data):
    if all_leaves(node): #we know it is decision node with all leaves
        turn, dic = turn_leaf(node, data)
        if turn:
            return Leaf(dic)

    elif isinstance(node, DecisionNode):# and (not isinstance(node.true_path, Leaf) or not isinstance(node.false_path, Leaf)):
        dict = split(data, node.col)
        #count = 0
        for key in node.children:#unique_items(orig_data, node.col):
            #count += 1
            #print(dict[key]["data"])
            if key in dict:
                node.children[key] = prune(node.children[key], dict[key]["data"])
            else:
                arr = []
                node.children[key] = prune(node.children[key], arr)

    if all_leaves(node): #we know it is decision node with all leaves
        turn, dic = turn_leaf(node, data)
        if turn:
            return Leaf(dic)
    return node


def classify(row, node):
    if isinstance(node, Leaf):
        #print(node.results)
        return node.results
    else: # then is decision node
        #print(node.col)
        #print(row[node.col])
        #print("next")
        #if node.children[row[node.col]] is None:
        #    return
        return classify(row, node.children[row[node.col]])



def print_leaf(counts):
    total = sum(counts.values()) * 1.0
    probs = {}
    for lbl in counts.keys():
        probs[lbl] = str(int(counts[lbl] / total * 100)) + "%"
    return probs


def stratify(data):
    #trainSize = int(len(data)*.80)
    val_size = int(len(data)*.20)
    #print(trainSize)
    #print(val_size)
    data = random.sample(data, len(data))
    #count = 0
    while not stratified(data, val_size):
        data = random.sample(data, len(data))
        #count += 1
        #print(count)
    return data[val_size:], data[:val_size]


def stratified(data, val_size): #assuming only two classes
    if len(num_classes(data[val_size:])) <= 1 or len(num_classes(data[:val_size])) <= 1:
        return False
    #matching = False
    data1, data2 = [], []
    for key in num_classes(data[val_size:]):
        data1.append(num_classes(data[val_size:])[key])
        data2.append(num_classes(data[:val_size])[key])
    if data1[0]/data1[1] <= data2[0]/data2[1]+0.05 and data1[0]/data1[1] >= data2[0]/data2[1]-0.05:
        return True
    else:
        return False


def to_csv(data, outfile):
    for row in data:
        for item in row:
            outfile.write(str(item) + ",")
        outfile.write("\n")


def print_accuracy(test, tree):
    count = 0
    for row in test:
        if (len(classify(row, tree)) == 1) and (classify(row, tree).get(row[0]) is not None):
            count += 1
    print(count/len(test))


def ret_accuracy(test, tree):
    count = 0
    for row in test:
        if (len(classify(row, tree)) == 1) and (classify(row, tree).get(row[0]) is not None):
            count += 1
    if len(test) == 0:
        return 0
    return(count/len(test))


monks_train = load("train monks.txt")
monks_test = load("test monks.txt")
tree = build_tree(monks_train)
depthprune = depth_prune_tree(monks_train, 6)
print("Base Decision Tree:")
print_tree(tree)
print("Accuracy of Base Decision Tree:")
print_accuracy(monks_test, tree)
monks_train, monks_val = stratify(monks_train)
postprune = build_tree(monks_train)
#print_accuracy(monks_test, postprune)
postprune = prune(postprune, monks_val)
print("\n\nPost-Pruned Tree:")
print_tree(postprune)
print("Accuracy of Post-Pruned Tree:")
print_accuracy(monks_test, postprune)
preprune = preprune_tree(monks_train, monks_val)
print("\n\nPre-Pruned Tree:")
print_tree(preprune)
print("Accuracy of Pre-Pruned Tree:")
print_accuracy(monks_test, preprune)
print("\n\nPre-Pruned Tree:")
print_tree(depthprune)
print("Accuracy of Depth-Restricted Tree:")
print_accuracy(monks_test, depthprune)
#print_accuracy(monks_test, tree)
