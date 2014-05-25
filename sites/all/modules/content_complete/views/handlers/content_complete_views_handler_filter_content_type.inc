<?php
// $Id: content_complete_views_handler_filter_content_type.inc,v 1.1.2.2.2.1 2010/10/13 13:11:30 pvhee Exp $

/**
 *  @file
 *  This defines a views handler to filter nodes by whether or not they're Content-Complete-enabled.
 */

class content_complete_views_handler_filter_content_type extends views_handler_filter_boolean_operator {
  function query() {
    $types = content_complete_get_enabled_types();
    $this->query->add_where(0, 'node.type', $types, empty($this->value) ? 'NOT IN' : 'IN'); 
  }
}