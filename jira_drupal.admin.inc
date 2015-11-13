<?php
module_load_include('inc', 'jira_drupal', 'jira_drupal.helpers');
module_load_include('inc', 'jira_drupal', 'jira_drupal.api');

function jira_drupal_admin_config($form, $form_state)
{
  $projects = array('0' => '--Select--') + jira_drupal_format_system_info(jira_drupal_jira_rest_get_projects());
  $issuetypes = array('0' => '--Select--') + jira_drupal_format_system_info(jira_drupal_jira_rest_get_issuetypes());

  $form = array(
    'project' => array(
      '#type' => 'select',
      '#title' => 'Project',
      '#options' => $projects,
      '#default_value' => variable_get('jira_drupal_jira_project'),
    ),
    'issuetype' => array(
      '#type' => 'select',
      '#title' => 'Issue Type',
      '#options' => $issuetypes,
      '#default_value' => variable_get('jira_drupal_jira_issuetype'),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => 'Submit',
    ),
  );

  return $form;
}

function jira_drupal_admin_config_validate($form, $form_state)
{
}

function jira_drupal_admin_config_submit($form, $form_state)
{
  if($form_state['values']['project'] != 0)
  {
    variable_set('jira_drupal_jira_project', $form_state['values']['project']);
  }

  if($form_state['values']['issuetype'] != 0)
  {
    variable_set('jira_drupal_jira_issuetype', $form_state['values']['issuetype']);
  }
}
