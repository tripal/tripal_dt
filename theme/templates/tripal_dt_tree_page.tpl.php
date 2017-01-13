<h2>Tree raw data</h2>
<?php
  echo theme('table', $table);
?>
<br/>
<h2>Tree JSON structure</h2>
<pre>
<?php
  echo json_encode(
    $tree,
    JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES
  );
?>
</pre>
<br/>
<h2>Tree</h2>
<div id="tripal_dt_tree">
</div>
<script>
  jQuery(function() {
    var tree_data = <?php print json_encode($tree); ?>;
    Drupal.tripal_dt.renderD3JSTree(tree_data);
  });
</script>
