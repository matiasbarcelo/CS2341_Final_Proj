# Assignment 4: Search Engine 

This is a longer group project! Here are the [Project Instructions](Project_Instructions.pdf).


**Objectives:**

* Work in a team using version control.
* Design a larger system, plan the implementation and submit multiple deliverables.
* Implement a search tree-based map.
* Implement a persistent data structure.


## Timeline 

You have about four weeks to complete the assignment. Start right away! **The lab TA will expect that you already have most of that week's work completed. Come with prepared questions and
be prepared to answer the TA's questions about your code.**

Suggested timeline (see Canvas for early deliverables):

* Week 1: Plan your timeline (see below); design you solution (first UML class diagram); 
    adapt the AVL tree from class to work as a map suitable for the inverted file index and provide tests (Catch2 test would be great).
* Week 2: Implement the document parser; build the indices. 
* Week 3: Implement the query processor; user interface; and add persistence to the index.
* Week 4: Finish code and write the documentation file.

# Data

The repository comes with a few example documents in the `sample_data` folder.
The full dataset can be downloaded here: https://www.kaggle.com/jeet2016/us-financial-news-articles

The goal is that your implementation can index the full dataset in a reasonable amount of time. You can also generate your own smaller datasets by copying some documents 
from the large dataset into a test folder.

# Submission 

## Documentation

Complete the file `answers.md` to describe your design, data structures, etc.


## Software

Your executable needs to be called `supersearch` and have the following command line interface.
The first argument is the command followed by additional information for the command. Here are examples:

Usage:

* Index all files in `<directory>` and store the index in one or several files:
  
  ```
  supersearch index <directory>
  ```

* Load the existing index and perform the following query:
  
  ```
  supersearch query "social network PERSON:cramer"
  ```

* Start a simple text-based user interface that lets the user create an index,
  save/load the index and perform multiple queries:
  
  ```
  supersearch ui
  ```

For the UI, you can display a simple menu like `Press i for creating a new index`, etc.


### Automatic Tests and Memory Tests
You need to describe test cases for:
- Unit tests for your AVL tree's function (Catch2)
- Tests for your AVL tree for memory leaks (don't forget to test the persistence functions)
- Unit tests for indexer (Catch2)
- System tests for performing queries (manual tests on small datasets to check if they find the correct documents)

**Hint:** For testing you should use a very small dataset (e.g., set of documents) where you know what the correct output is 
  and then a larger set to check that your data structures can handle more data.

## Bonus Work (max. of 10)

* During the presentation, we will test your implementation to determine the following. 
    1. The indexing and query speed on a smaller dataset.
    2. The correctness of the result for several queries on that dataset.
  For all teams with correct results, the top 10% of teams according to indexing and query speed will be awarded a bonus of +10. The next 10% will be awarded +5. Note that speed depends all on using the most appropriate and optimized data structures.
* To speed up query processing, you can implement your own hash map from scratch for the organization and person 
  index. (up to +5)
* Analytics for the corpus (e.g., report current indexing speed in documents/second, update the speed every 100 
  documents; 10 most popular terms, 10 least popular terms, number of terms in the index, ...) (up to +2)
* Implementation of very smart relevancy ranking ideas (up to +2)

## Compilation and Speed Tests

We will do the following on a Genuse server:

```
git clone <your repository>

cd <your repository>

cmake . -B build -DCMAKE_BUILD_TYPE=Release
cd build

make supersearch

time ./supersearch index <our test data path>
time ./supersearch query <our test queries>

./supersearch ui
```

Make sure that you test that this procedure works with the provided sample data.


## Grading Rubric

| Task                        | Points Possible | Points Awarded |
| :------------------         | --------------: | -------------: |
| UML diagram/design          | 10              |                |
| AVL tree map + tests        | 20              |                |
| AVL tree persistence + tests| 10              |                |
| Document parser / indexer   | 10              |                |
| Query processor + interface | 20              |                |
| Proper Memory Management    | 5               |                |
| Formatting, Comments, etc.  | 5               |                |
| Documentation               | 20              |                |
| Bonus Work                  | up to +10       |                |
| 48hr early submission bonus | +5              |                |

