## Similar By Terms 2 ##
module for Drupal  
created by Jeff Robbins / Lullabot
- - - - - - - - - - - - - - - - - -

This is a from-scratch rewrite of [Similar By Terms](http://drupal.org/project/similarterms) as a [Views](http://drupal.org/project/views) plug-in. The module creates lists of nodes sorted by their similarity to a given node based on commonality of taxonomy terms. Similar By Terms 2 uses Views to create its database queries and display its results. This means that you can define what fields to show, additional sorting, filters, displays, etc. Super flexible!

### Set up: ###

1) Put the module in your modules directory (usually sites/all/modules) and enable it.
2) Your now have a block called "Related Content" which you can place on your site. This block will *only* appear on node pages and it will list other content similar to this node based on taxonomy terms. The block will also not appear if the node has no taxonomy terms associated with it.

The simplest way to set things up is to add a free tagging vocabulary to the nodes which you'd like to show similarity and then add a bunch (more than 3, less than 20) terms to your content. Then enable the "Similar By Terms" block on the page. Ta da!

### Views integration: ###

If you would like to customize this block or create a new view listing content by similarity, visit admin/build/views. You'll find a view called "Similar By Terms". You can override the default view to customize the block or clone it if you want to create a new view.

- **Arguments** - Similar to what? First off, Similar By Terms needs to know what node it is going to show stuff similar to. This is done through the "Arguments" section of the Views interface. Adding *"Similar By Terms: Nid"* here will allow you to pass in a node id. You can usually set this to 'Provide default argument' > 'Node ID from URL' if you want to show a block on the same page as a node, or a tab on the node page. The settings here will also allow you to limit similarity to terms within certain vocabularies and you can choose whether to show the original node in the listing or not.

- **Sort criteria** - Similar By Terms's magic happens in the "Sort criteria". Simply add *"Similar By Terms: Similarity"* as your first sort criteria and results will be sorted by similarity to the node given as an argument to the view. Set sort order to "Descending" to show most similar stuff at the top. You may also want to provide secondary sorting, so that nodes with the same number of common terms are sorted either alphabetically, or by date.

- **Fields** - There's also a field titled *"Similar By Terms: Similarity"* which shows the commonality between the terms associated with the listed nodes and those of the node being passed in as an argument. This can be output either as a percentage (recommended), or as a raw count of the number of terms which the nodes have in common. This field is particularly useful in testing to make sure that you've got everything set up correctly.

Keep in mind that the more terms used, the better for finding similarity. However, more terms will create slower database queries. Be sure to cache blocks and Views output where possible.

### Example ###

    Given node (passed as argument):
      Node 1
        Terms: A, B, C, D, E, F
        (6 terms total)
  
    Results:
      Node 2
        Terms: [B, C, D, E, F,] G
        (5 terms in common with given = similarity 83%)
      Node 3
        Terms: [A, B, C, D, E,] H
        (5 terms in common  = similarity 83%)
      Node 4
        Terms: [B, C, D, F,] I, J, K, L, M
        (4 terms in common = similarity 67%)
      Node 5
        Terms: [A, B, C]
        (3 terms in common = similarity 50%)
      Node 6
        Terms: [A, B,] J, K, L, M, N, O, P, Q
        (2 terms in common = similarity 33%)
        

### Alternate Uses ###
Since this is a Views plug-in, it is possible to create listings of related images, videos, or just about anything which you can show with Views. Here are a few tricks you might try:

- __Related tab__: Create a (page display) tab called "Related" on node pages. This page shows teaser listings of similar content.
- __Advertising using Similar By Terms__: Create an "advertisement" node type with CCK fields for "image" and "URL". Add your "tags" vocabulary to this content type and create some ads. Create a block which shows the linked image of the most similar advertisement node.

Other ideas? Let us know!