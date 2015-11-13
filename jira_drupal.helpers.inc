<?php
module_load_include('inc', 'jira_drupal', 'jira_drupal.api');

function jira_drupal_format_system_info($json)
{
  $object = drupal_json_decode($json);
  $object_formatted = array();
  foreach($object as $o)
  {
    $object_formatted[$o['id']] = $o['name'];
  }

  return $object_formatted;
}

function jira_drupal_format_fields($fields)
{
  $formatFields = array();
  foreach($fields as $key => $field)
  {
    $formatFields[$key] = $field->name;
  }

  return $formatFields;
}

function jira_drupal_field_data_string($issuedata, $fieldname)
{
  if(isset($issuedata->fields->$fieldname) && !empty($issuedata->fields->$fieldname))
  {
    return $issuedata->fields->$fieldname;
  }
  else
  {
    return "No value";
  }
}

function jira_drupal_field_data_array($issuedata, $fieldname)
{
  if(isset($issuedata->fields->$fieldname) && !empty($issuedata->fields->$fieldname))
  {
    $value = $issuedata->fields->$fieldname;
    if(isset($value->name))
    {
      return $value->name;
    }
    else if(isset($value->value))
    {
      return $value->value;
    }
  }
  else
  {
    return "No value";
  }
}

function jira_drupal_field_data_date($issuedata, $fieldname)
{
  if(isset($issuedata->fields->$fieldname) && !empty($issuedata->fields->$fieldname))
  {
    return date('g:ia F jS, o', strtotime($issuedata->fields->$fieldname));
  }
  else
  {
    return "No value";
  }
}

function jira_drupal_field_data_file($issuedata, $fieldname)
{
  if(isset($issuedata->fields->$fieldname) && !empty($issuedata->fields->$fieldname))
  {
    $html = '';
    foreach($issuedata->fields->$fieldname as $key => $a) 
    {
      $html .= "<div class='jira_drupal_field_" . $fieldname . "' id='jira_drupal_field_" . $fieldname . "_" . $key . "'>";
      $html .= "<a href='" . $a->content . "'>" . $a->filename . "</a>";
      $html .= "</div>";
    }

    return $html;
  }
  else
  {
    return "No value";
  }
}

function jira_drupal_view_field_string($jira_key, $jira_field, $issuedata)
{
  if(isset($jira_field->allowedValues))
  {
    return jira_drupal_view_field_array($jira_key, $jira_field, $issuedata);
  }
  else
  {
    $field = array(
      '#title' => $jira_field->name,
      '#markup' => "<div>" . $jira_field->name . ": " . jira_drupal_field_data_string($issuedata, $jira_key) . "</div>",
    );
  }

  return $field;
}

function jira_drupal_view_field_array($jira_key, $jira_field, $issuedata)
{
  if(isset($jira_field->schema->items) && $jira_field->schema->items == 'attachment')
  {
    return jira_drupal_view_field_file($jira_key, $jira_field, $issuedata);
  }
  else
  {
    $field = array(
      '#title' => $jira_field->name,
      '#type' => 'markup',
      '#markup' => "<div>" . $jira_field->name . ": " . jira_drupal_field_data_array($issuedata, $jira_key) . "</div>",
    );

    return $field;
  }
}

function jira_drupal_view_field_date($jira_key, $jira_field, $issuedata)
{
  $field = array(
    '#type' => 'markup', 
    '#title' => $jira_field->name,
    '#markup' => "<div>" . $jira_field->name . ": " . jira_drupal_field_data_date($issuedata, $jira_key) . "</div>",
  );

  return $field;
}

function jira_drupal_view_field_file($jira_key, $jira_field, $issuedata)
{
  $field = array(
    '#type' => 'markup',
    '#title' => $jira_field->name,
    '#markup' => "<div>" . $jira_field->name . ": " . jira_drupal_field_data_file($issuedata, $jira_key) . "</div>", 
  );

  return $field;
}

function jira_drupal_view_field_priority($jira_key, $jira_field, $issuedata)
{
  return jira_drupal_view_field_array($jira_key, $jira_field, $issuedata);
}

function jira_drupal_build_field_string($jira_key, $jira_field, $issuedata = null)
{
  if(isset($jira_field->allowedValues))
  {
    return jira_drupal_build_field_array($jira_key, $jira_field, $issuedata);
  }
  else
  {
    $field = array(
      '#title' => $jira_field['name'],
      '#type' => 'textarea',
    );

    if($issuedata != null)
    {
      $field['#default_value'] = jira_drupal_field_data_string($issuedata, $jira_key);
    }
  }

  return $field;
}

function jira_drupal_build_field_number($jira_key, $jira_field, $issuedata = null)
{
  return jira_drupal_build_field_string($jira_key, $jira_field, $issuedata);
}

function jira_drupal_build_field_user($jira_key, $jira_field, $issuedata = null)
{
  return jira_drupal_build_field_string($jira_key, $jira_field, $issuedata);
}

function jira_drupal_build_field_array($jira_key, $jira_field, $issuedata = null)
{
  if(isset($jira_field['schema']['items']) && $jira_field['schema']['items'] == 'attachment')
  {
    return jira_drupal_build_field_file($jira_key, $jira_field, $issuedata);
  }
  else
  {
    $values = array();
    foreach($jira_field['allowedValues'] as $key => $av)
    {
      if(isset($av['id']))
      {
        $key = $av['id'];
      }

      if(isset($av['name']))
      {
        $values[$key] = $av['name'];
      }
      else if(isset($av['value']))
      {
        $values[$key] = $av['value'];
      }
    }

    $field = array(
      '#title' => $jira_field['name'],
      '#type' => 'select',
      '#multiple' => TRUE,
      '#options' => $values,
    );

    if($issuedata != null)
    {
      $field['#default_value'] = jira_drupal_field_data_array($issuedata, $jira_key);
    }

    return $field;
  }
}

function jira_drupal_build_field_date($jira_key, $jira_field, $issuedata = null)
{
  $field = array(
    '#type' => 'date', 
    '#title' => check_plain($jira_field['name']), 
  );

  if($issuedata != null)
  {
    $field['#default_value'] = jira_drupal_field_data_date($issuedata, $jira_key);
  }

  return $field;
}

function jira_drupal_build_field_file($jira_key, $jira_field, $issuedata)
{
  $field = array(
    '#type' => 'container',
    '#access' => TRUE,
    'file_upload' => array(
    ),
  );

  return $field;
}

function jira_drupal_build_field_priority($jira_key, $jira_field, $issuedata = null)
{
  $values = array();
  foreach($jira_field['allowedValues'] as $key => $av)
  {
    if(isset($av['name']))
    {
      $values[$av['name']] = $av['name'];
    }
    else if(isset($av['value']))
    {
      $values[$av['value']] = $av['value'];
    }
  }

  $field = array(
    '#title' => $jira_field['name'],
    '#type' => 'select',
    '#options' => $values,
  );

  if($issuedata != null)
  {
    $field['#default_value'] = jira_drupal_field_data_date($issuedata, $jira_key);
  }
  else
  {
    $field['#default_value'] = 'Normal';
  }

  return $field;
}

function jira_drupal_submit_field_string($value, $jira_field)
{
  if(isset($jira_field->allowedValues))
  {
    return jira_drupal_submit_field_array($value, $jira_field);
  }
  else
  {
    return $value;
  }
}

function jira_drupal_submit_field_number($value, $jira_field)
{
  return jira_drupal_submit_field_string($value, $jira_field);
}

function jira_drupal_submit_field_user($value, $jira_field)
{
  $name = 'requester';
  $search_results = drupal_json_decode(jira_drupal_jira_rest_search_user($value));
  if(count($search_results) == 1 && isset($search_results[0]['name']) && !empty($search_results[0]['name']))
  {   
    $name = $search_results[0]['name'];
  }   

  return array('name' => $name);
}

function jira_drupal_submit_field_array($value, $jira_field)
{
  $data = array();
  foreach($value as $v)
  {
    $data[] = array('id' => $v);
  }
  return $data;
}

function jira_drupal_submit_field_date($value, $jira_field)
{
  return $value['year'] . '-' .$value['month'] . '-' . $value['day'];
}

function jira_drupal_submit_field_file($value, $jira_field)
{
  //This space intentionally left blank
}

function jira_drupal_submit_field_priority($value, $jira_field)
{
  return array('name' => $value);
}
