<?php

/**
 * @file
 * Administrative page callbacks for the snowplow module.
 */

/**
 * Implements hook_admin_settings(). for module settings configuration.
 */
function snowplow_admin_settings_form($form_state) {

  $form['app'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );

  $form['app']['snowplow_app_id'] = array(
    '#title' => t('Snowplow App ID'),
    '#type' => 'textfield',
    '#default_value' => variable_get('snowplow_app_id', 'APP-ID'),
    '#size' => 15,
    '#maxlength' => 20,
    '#required' => TRUE,
    '#description' => t('This id will be set on the tracker.'),
  );

  $form['app']['snowplow_app_endpoint'] = array(
    '#title' => t('Endpoint URL'),
    '#type' => 'textfield',
    '#default_value' => variable_get('snowplow_app_endpoint', ''),
    '#size' => 128,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#description' => t('Enter the endpoint where the events will be tracked, the format of this url is important and will be the most common cause of missing events if incorrectly tracked. (Leave off the http://)'),
  );

  // Visibility settings.
  $form['tracking_title'] = array(
    '#type' => 'item',
    '#title' => t('Tracking scope'),
  );

  $form['tracking'] = array(
    '#type' => 'vertical_tabs',
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'snowplow') . '/snowplow.admin.js'),
    ),
  );

  $form['tracking']['login_tracking'] = array(
    '#type' => 'fieldset',
    '#title' => t('Login'),
  );

  $form['tracking']['login_tracking']['snowplow_track_login'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track logins?'),
    '#default_value' => variable_get('snowplow_track_login', TRUE),
    '#description' => t('Track user logins to snowplow.'),
    ),
  );

  $form['tracking']['logout_tracking'] = array(
    '#type' => 'fieldset',
    '#title' => t('Logout'),
  );

  $form['tracking']['logout_tracking']['snowplow_track_logout'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track logouts?'),
    '#default_value' => variable_get('snowplow_track_logout', TRUE),
    '#description' => t('Track user logouts to snowplow.'),
    ),
  );

  $form['tracking']['new_user_tracking'] = array(
    '#type' => 'fieldset',
    '#title' => t('New User'),
  );

  $form['tracking']['new_user_tracking']['snowplow_track_new_user'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track new users?'),
    '#default_value' => variable_get('snowplow_track_new_user', TRUE),
    '#description' => t('Track new user registrations to snowplow'),
    ),
  );

  // Message specific configurations.
  $form['tracking']['messagetracking'] = array(
    '#type' => 'fieldset',
    '#title' => t('Messages'),
  );

  $form['tracking']['login_tracking']['snowplow_trackmessages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track messages of type'),
    '#default_value' => variable_get('snowplow_track_login', TRUE),
    '#description' => t('Track user messages to snowplow'),
    ),
  );

  $form['tracking']['messagetracking']['snowplow_trackmessages'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Track messages of type'),
    '#default_value' => variable_get('snowplow_trackmessages', array()),
    '#description' => t('Track user messages to snowplow'),
    '#options' => array(
      'status' => t('Status message'),
      'warning' => t('Warning message'),
      'error' => t('Error message'),
    ),
  );

  return system_settings_form($form);

}

