## UML Diagram

![UML Class Diagram](Project4_UML.png)

## Description of Classes

**supersearch:** My UI implementation and what will be used in the command line to use application. This is a basic driver.

**QueryEngine:** Used to stip queries to their stams and remove stop words from queries. Will also return search results to SuperSearch UI.

**DocumentParser:** Parses file in directory for index or updates index using persistent index file

**Index:** Holds the AVL Trees (or inverted file index) for words, people, and orgs used to generate search results

**IndexHandler:** Determines what is relevent using index class

**SearchEngine:** Object that instantiates classes for ease of use by driver/UI. 

**Document:** Contains document information for later display

**AVLTree & AVLNode:** AVL implementation

## Class interaction

* **supersearch** gives **DocumentParser** dir of files to parse or persistent index file to use
* **DocumentParser** works with **Index** to make inverted file indexes for words, people, and orgs which are really AVL Tree data structures that act as maps with values consisting of std::vectors of **Document**. During this process stemming and stop word removal is done to words.
* **supersearch** sends query to **QueryEngine**, which does a similar process to **DocumentParser** in stripping the query search of stop words and stemming the words.
* **QueryEngine** sends stipped query to **IndexHandler**, which uses **Index**'s inverted file index's to generate a search result based of some sort of relevency metric.
* **IndexHandler** returns search result to **QueryEngine** which in turn returns the result to **supersearch** for display to the user.

## Schedule

| Task                                                                             | Planned completion | Actual completion | Time spent in hrs      |
| -------------------------------------------------------------------------------- | :-----------------:| :---------------: | :--------------------: |
| AVL Tree Implementation and Catch 2 Tests. Finish UML Design                     | April 10           |                   |                        |
| Implement the document parser; build the indices.                                | April 13           |                   |                        |
| Implement the query processor; user interface; and add persistence to the index. | April 20           |                   |                        |
| Everything else                                                                  | April 27           |                   |                        |