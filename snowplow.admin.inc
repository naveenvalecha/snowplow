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
    '#description' => t('Enter the endpoint where the events will be tracked too.'),
  );

  // Visibility settings.
  $form['tracking_title'] = array(
    '#type' => 'item',
    '#title' => t('Tracking scope'),
  );

  $form['tracking'] = array(
    '#type' => 'vertical_tabs',
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'googleanalytics') . '/googleanalytics.admin.js'),
    ),
  );

  $form['tracking']['domain_tracking'] = array(
    '#type' => 'fieldset',
    '#title' => t('Domains'),
  );

  global $cookie_domain;
  $multiple_sub_domains = array();
  foreach (array('www', 'app', 'shop') as $subdomain) {
    if (count(explode('.', $cookie_domain)) > 2 && !is_numeric(str_replace('.', '', $cookie_domain))) {
      $multiple_sub_domains[] = $subdomain . $cookie_domain;
    }
    // IP addresses or localhost.
    else {
      $multiple_sub_domains[] = $subdomain . '.example.com';
    }
  }

  $multiple_toplevel_domains = array();
  foreach (array('.com', '.net', '.org') as $tldomain) {
    $host = $_SERVER['HTTP_HOST'];
    $domain = substr($host, 0, strrpos($host, '.'));
    if (count(explode('.', $host)) > 2 && !is_numeric(str_replace('.', '', $host))) {
      $multiple_toplevel_domains[] = $domain . $tldomain;
    }
    // IP addresses or localhost
    else {
      $multiple_toplevel_domains[] = 'www.example' . $tldomain;
    }
  }

  $form['tracking']['domain_tracking']['googleanalytics_domain_mode'] = array(
    '#type' => 'radios',
    '#title' => t('What are you tracking?'),
    '#options' => array(
      0 => t('A single domain (default)') . '<div class="description">' . t('Domain: @domain', array('@domain' => $_SERVER['HTTP_HOST'])) . '</div>',
      1 => t('One domain with multiple subdomains') . '<div class="description">' . t('Examples: @domains', array('@domains' => implode(', ', $multiple_sub_domains))) . '</div>',
      2 => t('Multiple top-level domains') . '<div class="description">' . t('Examples: @domains', array('@domains' => implode(', ', $multiple_toplevel_domains))) . '</div>',
    ),
    '#default_value' => variable_get('googleanalytics_domain_mode', 0),
  );
  $form['tracking']['domain_tracking']['googleanalytics_cross_domains'] = array(
    '#title' => t('List of top-level domains'),
    '#type' => 'textarea',
    '#default_value' => variable_get('googleanalytics_cross_domains', ''),
    '#description' => t('If you selected "Multiple top-level domains" above, enter all related top-level domains. Add one domain per line. By default, the data in your reports only includes the path and name of the page, and not the domain name. For more information see section <em>Show separate domain names</em> in <a href="@url">Tracking Multiple Domains</a>.', array('@url' => 'https://support.google.com/analytics/answer/1034342')),
    '#states' => array(
      'enabled' => array(
        ':input[name="googleanalytics_domain_mode"]' => array('value' => '2'),
      ),
      'required' => array(
        ':input[name="googleanalytics_domain_mode"]' => array('value' => '2'),
      ),
    ),
  );

  // Message specific configurations.
  $form['tracking']['messagetracking'] = array(
    '#type' => 'fieldset',
    '#title' => t('Messages'),
  );

  $form['tracking']['messagetracking']['googleanalytics_trackmessages'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Track messages of type'),
    '#default_value' => variable_get('googleanalytics_trackmessages', array()),
    '#description' => t('This will track the selected message types shown to users. Tracking of form validation errors may help you identifying usability issues in your site. For each visit (user session), a maximum of approximately 500 combined GATC requests (both events and page views) can be tracked. Every message is tracked as one individual event. Note that - as the number of events in a session approaches the limit - additional events might not be tracked. Messages from excluded pages cannot be tracked.'),
    '#options' => array(
      'status' => t('Status message'),
      'warning' => t('Warning message'),
      'error' => t('Error message'),
    ),
  );

  return system_settings_form($form);

}

