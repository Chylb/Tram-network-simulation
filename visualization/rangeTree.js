class RangeTree {
    constructor(keys, values) {
        this.leaves = [];

        let prevLeaf;
        let leaf;
        for (let i = 0; i < keys.length; i++) {
            leaf = new Leaf(keys[i], values[i], prevLeaf);
            prevLeaf = leaf;
            this.leaves.push(leaf);
        }

        let height = 0;
        let nodes;
        let upperNodes = this.leaves;

        while (upperNodes.length > 1) {
            nodes = upperNodes;
            upperNodes = [];

            for (let i = 0; i < nodes.length; i += 2) {
                let leftNode = nodes[i];
                let rightNode = nodes[i + 1];

                let upNode = new Node(leftNode, rightNode);
                upperNodes.push(upNode);
            }
            height++;
        }
        this.root = upperNodes[0];
        this.height = height;
    }

    find(key) {
        let node = this.root;

        for (let i = 0; i < this.height; i++) {
            if (key < node.key)
                node = node.left;
            else if (node.right !== undefined) {
                node = node.right;
            }
            else {
                return this.leaves[this.leaves.length - 1].val;
            }
        }
        const leftLeaf = node.leftLeaf;
        if (leftLeaf !== undefined)
            return leftLeaf.val;

        if (this.height > 1)
            return undefined;
        else {
            if (key >= this.leaves[0].key)
                return this.leaves[0].val;
            else
                return undefined;
        }
    }
}

class Leaf {
    constructor(key, val, leftLeaf) {
        this.key = key;
        this.val = val;
        this.leftLeaf = leftLeaf;
        this.subTreeMaxKey = key;
    }
}

class Node {
    constructor(left, right) {
        this.key = left.subTreeMaxKey;
        this.left = left;
        this.right = right;

        if (right !== undefined)
            this.subTreeMaxKey = right.subTreeMaxKey;
        else
            this.subTreeMaxKey = left.subTreeMaxKey;
    }
}
