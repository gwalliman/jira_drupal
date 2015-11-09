<?php

function jira_drupal_jira_rest_get($url_append)
{
  $username = variable_get('jira_rest_username');
  $password = variable_get('jira_rest_password');
  $ch = jira_rest_get_curl_resource($username, $password, $url_append);
  return jira_rest_curl_execute($ch);
}

function jira_drupal_jira_rest_field_list()
{
  return jira_drupal_jira_rest_get('/field');
}

function jira_drupal_jira_rest_status()
{
  return jira_drupal_jira_rest_get('/status');
}

function jira_drupal_jira_rest_project_status($status)
{
  return jira_drupal_jira_rest_get('/project/' . $status . '/statuses');
}

function jira_drupal_jira_rest_get_transitions($issuekey)
{
  return jira_drupal_jira_rest_get('/issue/' . $issuekey . '/transitions');
}

function jira_drupal_jira_rest_get_transition($issuekey, $status_id)
{
  $transitions = jira_drupal_jira_rest_get('/issue/' . $issuekey . '/transitions');
  foreach($transitions->transitions as $t)
  {
    if($t->to->id == $status_id)
    {
      return $t;
    }
  }
}

function jira_drupal_sfe_status_labels()
{
  $statuses = jira_drupal_sfe_statuses();
  $status_format = array('N/A');
  foreach($statuses as $s)
  {
    $status_format[$s->id] = $s->name;
  }
  return $status_format;
}

function jira_drupal_sfe_status($status_label)
{
  $statuses = jira_drupal_sfe_statuses();
  foreach($statuses as $status)
  {
    if($status->name == $status_label)
    {
      return $status;
    }
  }
}

function jira_drupal_sfe_statuses()
{
  $statuses = jira_drupal_jira_rest_project_status('SFE');
  foreach($statuses as $status)
  {
    if($status->name == 'New Development')
    {
      return $status->statuses;
    }
  }
}

function jira_drupal_sfe_fields()
{
  //TODO: comments
  $projects = jira_drupal_jira_rest_get('/issue/createmeta?projectKeys=SFE&expand=projects.issuetypes.fields');
  $issuetypes = $projects->projects['0']->issuetypes;
  foreach($issuetypes as $issue)
  {
    if($issue->name == 'Salesforce Development ')
    {
      $fields = $issue->fields;
      unset($fields->project);
      unset($fields->issuetype);
      return $fields;
    }
  }
}

function jira_drupal_sfe_field_data($fieldList, $field_id)
{
  foreach($fieldList as $key => $f)
  {
    if($key == $field_id)
    {
      return $f;
    }
  }
}

function jira_drupal_jira_rest_set_transition($issuekey, $transition_id)
{
  $data = array(
    "transition" => array('id' => $transition_id),
  );  

  $username = variable_get('jira_rest_username');
  $password = variable_get('jira_rest_password');

  $ch = jira_rest_get_curl_resource($username, $password, "/issue/" . $issuekey . "/transitions");
  curl_setopt($ch, CURLOPT_POST, TRUE);
  curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
  curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-type: application/json'));
  return jira_rest_curl_execute($ch);
}

function jira_drupal_jira_rest_add_comment($issuekey, $comment)
{
  $data = array(
    "body" => $comment,
  );  

  $username = variable_get('jira_rest_username');
  $password = variable_get('jira_rest_password');

  $ch = jira_rest_get_curl_resource($username, $password, "/issue/" . $issuekey . "/comment");
  curl_setopt($ch, CURLOPT_POST, TRUE);
  curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
  curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-type: application/json'));
  return jira_rest_curl_execute($ch);
}

function jira_drupal_jira_rest_get_full_issue($issuekey)
{
  $username = variable_get('jira_rest_username');
  $password = variable_get('jira_rest_password');
  return jira_rest_getfullissue($username, $password, $issuekey); 
}

function jira_drupal_jira_rest_search($jql)
{
  $username = variable_get('jira_rest_username');
  $password = variable_get('jira_rest_password');
  return jira_rest_searchissue($username, $password, $jql);
}
