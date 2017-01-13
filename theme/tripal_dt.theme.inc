<?php

/**
 * @file
 * This file should contain all Drupal hooks for theming content.
 *
 */

/**
 * Implements hook_preprocess_HOOK().
 *
 * Used to alter or add to theme variables. The variables are passed into
 * templates when processing. This function organizes the relationships
 * into more simple structures for parsing in the template file.
 *
 * @ingroup tripal_dt
 */
function tripal_dt_preprocess_tripal_dt_tree(&$variables) {
  // Adds the appropriate JS and CSS specific to our page.
  // Note: those scripts won't be loaded on other pages!
  drupal_add_js(drupal_get_path('module', 'tripal_dt') .'/theme/js/d3.min.js');
  drupal_add_js(drupal_get_path('module', 'tripal_dt') .'/theme/js/d3.dndtree.js');
  drupal_add_css(drupal_get_path('module', 'tripal_dt') .'/theme/css/tripal_dt.css');

  $phylotree_id = 1; // Our dataset phylotree_id.

  // Loads phylonode records from the database.
  // Note: this process can be long/heavy so it might be a good idea to use
  // Drupal cache features and load the structure from cache if it has already
  // been generated once and saved into cache.
  $columns = array('phylonode_id', 'parent_phylonode_id', 'label');
  $values = array(
    'phylotree_id' => $phylotree_id,
  );
  $options = array(
    'order_by' => array(
      'phylonode_id' => 'ASC',
    ),
  );
  $phylonodes = chado_select_record(
    'phylonode',
    $columns,
    $values,
    $options
  );

  // Raw data table stuff.
  $header = array(
   t('phylonode_id'),
   t('Parent'),
   t('Name'),
  );
  $rows = array();

  // JSON data stuff.
  $tree_data = array(); // Store D3 tree structure.
  $node_lookup = array(); // Associate a Chado ID to a D3 node ID.
  $node_index = 1; // Used to generate D3 IDs starting from 1.

  // Process each node and fill the data.
  foreach ($phylonodes as $phylonode) {
    // Fill raw data table.
    $rows[] = array(
      $phylonode->phylonode_id,
      $phylonode->parent_phylonode_id,
      $phylonode->label,
    );

    // Generate the tree structure.
    // Get current phylnode Chado ID.
    $phylonode_id = $phylonode->phylonode_id;
    // Fill lookup table.
    $node_lookup[$phylonode_id] = $node_index;
    // Store node data.
    $tree_data[$node_index] = array(
      'id' => $node_index,
      'name' => $phylonode->label,
    );
    // Check if node has a parent and if so, add it to its parent.
    if ($parent_id = $phylonode->parent_phylonode_id) {
      // Is it the first child?
      $parent_node_id = $node_lookup[$parent_id];
      if (!array_key_exists('children', $tree_data[$parent_node_id])) {
        // Yes, add a new children array to parent.
        $tree_data[$parent_node_id]['children'] = array(
          &$tree_data[$node_index]
        );
      }
      else {
        // No, add child to its siblings.
        $tree_data[$parent_node_id]['children'][] =
          &$tree_data[$node_index];
      }
    }
    // Next node.
    ++$node_index;
  }

  $variables['table'] = array('header' => $header, 'rows' => $rows);
  $variables['tree'] = $tree_data[1];
}
