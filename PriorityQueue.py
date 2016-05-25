""" ========================================================================
PriorityQueue.py

Created: March 24, 2016
Author: Susan Fox

Creates a priority queue, representing it as a simple heap. In this case the
data in the queue are objects that has a method to report its 
======================================================================== """


class PQueue:
    """Priority queue, where each item has a priority and associated data.
    Implemented with a MinHeap, which is internal to the class"""

    def __init__(self):
        """Create an initially empty heap, just a list with size zero"""
        self.heap = []
        self.size = 0


    def getSize(self):
        """Returns the size of the heap,the number of elements in it"""
        return self.size


    def isEmpty(self):
        """Checks if the size is zero.  If so, then it returns True else False"""
        return (self.size == 0)


    def push(self, priority, value):
        """Takes two inputs, a priority (which must be a number) and a value, which may
        be any kind of data.  It adds the new priority and value to the end of the heap,
        and then walks the data up the heap to the point where it forms a valid heap"""
        self.heap.append(PQNode(priority, value))
        self.size = self.size + 1
        self._walkUp(self.size - 1)


    def _walkUp(self, index):
        """A helper for enqueue, this method takes as input the index of the node needing
        to be walked up the heap.  It swaps the node with its parent if its priority is less than
        the parent's.  If it reaches the top of the heap, it stops, too."""
        inPlace = False
        while (not(index == 0) and not(inPlace)):
            parentIndex = self._parent(index)
            curr = self.heap[index]
            par = self.heap[parentIndex]
            if curr.getPriority() > par.getPriority():
                inPlace = True
            else:
                self.heap[index] = par
                self.heap[parentIndex] = curr
                index = parentIndex


    def getFirst(self):
        """"This returns the priority and value at the top of the heap, if there are values
        in the heap.  Otherwise it returns None"""
        if self.size > 0:
            topNode = self.heap[0]
            return topNode.getPriority(), topNode.getValue()
        else:
            return None, None


    def pop(self):
        """This removes the top element of the heap.  It moves the last element of the heap, the
        rightmost leaf, to the root position, then walks it down the tree until a valid heap
        is re-established"""
        if self.size == 0:
            return
        elif self.size == 1:
            self.size = self.size - 1
            self.heap = []
        else:
            self.size = self.size - 1
            lastItem = self.heap.pop(self.size)
            self.heap[0] = lastItem
            self._walkDown(0)


    def _walkDown(self, index):
        """A helper for dequeue, this takes as input the index of the node being walked
        down the heap.  It first looks at the children of this node, and determines which of them
        has smaller priority.  It then compares this node's priority with the smaller of the children
        and swaps them if the child is smaller.  It repeats until either both children have larger
        priorities, or it hits a leaf"""
        inPlace = False
        leftInd = self._leftChild(index)
        rightInd = self._rightChild(index)
        while (not(leftInd >= self.size) and not(inPlace)):
            curr = self.heap[index]

            if (rightInd >= self.size):
                minInd = leftInd
                minVal = self.heap[leftInd]
            else:
                leftVal = self.heap[leftInd]
                rightVal = self.heap[rightInd]
                if leftVal.getPriority() < rightVal.getPriority():
                    minInd = leftInd
                    minVal = leftVal
                else:
                    minInd = rightInd
                    minVal = rightVal
                
            if curr.getPriority() < minVal.getPriority():
                inPlace = True
            else:
                self.heap[minInd] = curr
                self.heap[index] = minVal
                index = minInd
                leftInd = self._leftChild(index)
                rightInd = self._rightChild(index)
                

    def removeNode(self, value, comparer):
        """Given a value that is in the heap, and a function for comparing two values,
        search for it and remove it.
        Removing it requires (1) finding the node, (2) replacing the node by the last leaf,
        and (3) walking the leaf down from that place."""
        if self.size == 1:
            # special case when only one value
            if comparer(self.heap[0].getValue(), value):
                self.size = self.size - 1
                self.heap = []
        else:
            for i in range(self.size):
                nextNode = self.heap[i]
                if comparer(nextNode.getValue(), value):
                    self.size = self.size - 1
                    lastItem = self.heap.pop(self.size)
                    if i != self.size:
                        self.heap[i] = lastItem
                        self.walkDown(i)
                    break
                
                
                
    def findMatch(self, value, comparer):
        """Given a value, and a function to use for comparing values, it searches the queue for a 
        matching value, returning it if found, or returning False if not found"""
        foundInd = self._matchIndex(value, comparer)
        if foundInd == -1:
            return False
        else:
            return self.heap[foundInd].getValue()
            
        
    def _matchIndex(self, value, comparer):
        """Given a value, and a function to use for comparing values, it searches the queue for a 
        matching value, returning its index position if found, or False if not found"""
        for i in range(self.size):
            nextNode = self.heap[i]
            if comparer(value, nextNode.getValue()):
                return i
        return -1
    
    
            
    def _parent(self, index):
        """Given an index into the heap, return the index of the parent node. Does no checking
        for bad input"""
        return (index - 1) // 2

    def _leftChild(self, index):
        """Given an index into the heap, returns the index of the node's left child.  This value
        may be greater than the size of the heap, if the indexed node is a left"""
        return (index * 2) + 1

    def _rightChild(self, index):
        """Given an index into the heap, returns the index of the node's right child.  This value
        may be greater than the size of the heap, if the indexed node has at most one child"""
        return (index + 1) * 2
# End of class PQueue


class PQNode:
    """This is a simple class to package up the priority and value for putting in the priority queue"""
    
    
    def __init__(self, priority, value):
        self.priority = priority
        self.value = value
        
        
    def getPriority(self):
        return self.priority
    
    def getValue(self):
        return self.value
    
# End of class PQNode



def main():

    pq = PQueue()

    print(pq.getSize())
    print(pq.isEmpty())

    pq.enqueue(5, 'apple')
    pq.enqueue(3, 'tom')
    pq.enqueue(8, 'skipping')
    pq.enqueue(2, 'is')

    print(pq.getSize())
    print(pq.isEmpty())
    print(pq.getFirst())

    pq.dequeue()
    print(pq.getSize())
    print(pq.isEmpty())
    print(pq.getFirst())

    pq.dequeue()
    print(pq.getSize())
    print(pq.isEmpty())
    print(pq.getFirst())

    pq.dequeue()
    print(pq.getSize())
    print(pq.isEmpty())
    print(pq.getFirst())

    pq.dequeue()
    print(pq.getSize())
    print(pq.isEmpty())
    print(pq.getFirst())
#end main

    
