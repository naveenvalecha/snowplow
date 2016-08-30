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

  // Page specific visibility configurations.
  $php_access = user_access('use PHP for tracking visibility');
  $visibility = variable_get('googleanalytics_visibility_pages', 0);
  $pages = variable_get('googleanalytics_pages', GOOGLEANALYTICS_PAGES);

  $form['tracking']['page_vis_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pages'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  if ($visibility == 2 && !$php_access) {
    $form['tracking']['page_vis_settings'] = array();
    $form['tracking']['page_vis_settings']['googleanalytics_visibility_pages'] = array('#type' => 'value', '#value' => 2);
    $form['tracking']['page_vis_settings']['googleanalytics_pages'] = array('#type' => 'value', '#value' => $pages);
  }
  else {
    $options = array(
      t('Every page except the listed pages'),
      t('The listed pages only'),
    );
    $description = t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

    if (module_exists('php') && $php_access) {
      $options[] = t('Pages on which this PHP code returns <code>TRUE</code> (experts only)');
      $title = t('Pages or PHP code');
      $description .= ' ' . t('If the PHP option is chosen, enter PHP code between %php. Note that executing incorrect PHP code can break your Drupal site.', array('%php' => '<?php ?>'));
    }
    else {
      $title = t('Pages');
    }
    $form['tracking']['page_vis_settings']['googleanalytics_visibility_pages'] = array(
      '#type' => 'radios',
      '#title' => t('Add tracking to specific pages'),
      '#options' => $options,
      '#default_value' => $visibility,
    );
    $form['tracking']['page_vis_settings']['googleanalytics_pages'] = array(
      '#type' => 'textarea',
      '#title' => $title,
      '#title_display' => 'invisible',
      '#default_value' => $pages,
      '#description' => $description,
      '#rows' => 10,
    );
  }

  // Render the role overview.
  $form['tracking']['role_vis_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Roles'),
  );

  $form['tracking']['role_vis_settings']['googleanalytics_visibility_roles'] = array(
    '#type' => 'radios',
    '#title' => t('Add tracking for specific roles'),
    '#options' => array(
      t('Add to the selected roles only'),
      t('Add to every role except the selected ones'),
    ),
    '#default_value' => variable_get('googleanalytics_visibility_roles', 0),
  );

  $role_options = array_map('check_plain', user_roles());
  $form['tracking']['role_vis_settings']['googleanalytics_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles'),
    '#default_value' => variable_get('googleanalytics_roles', array()),
    '#options' => $role_options,
    '#description' => t('If none of the roles are selected, all users will be tracked. If a user has any of the roles checked, that user will be tracked (or excluded, depending on the setting above).'),
  );

  // Standard tracking configurations.
  $form['tracking']['user_vis_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Users'),
  );
  $t_permission = array('%permission' => t('opt-in or out of tracking'));
  $form['tracking']['user_vis_settings']['googleanalytics_custom'] = array(
    '#type' => 'radios',
    '#title' => t('Allow users to customize tracking on their account page'),
    '#options' => array(
      t('No customization allowed'),
      t('Tracking on by default, users with %permission permission can opt out', $t_permission),
      t('Tracking off by default, users with %permission permission can opt in', $t_permission),
    ),
    '#default_value' => variable_get('googleanalytics_custom', 0),
  );
  $form['tracking']['user_vis_settings']['googleanalytics_trackuserid'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track User ID'),
    '#default_value' => variable_get('googleanalytics_trackuserid', 0),
    '#description' => t('User ID enables the analysis of groups of sessions, across devices, using a unique, persistent, and non-personally identifiable ID string representing a user. <a href="@url">Learn more about the benefits of using User ID</a>.', array('@url' => 'https://support.google.com/analytics/answer/3123663')),
  );

  // Link specific configurations.
  $form['tracking']['linktracking'] = array(
    '#type' => 'fieldset',
    '#title' => t('Links and downloads'),
  );
  $form['tracking']['linktracking']['googleanalytics_trackoutbound'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track clicks on outbound links'),
    '#default_value' => variable_get('googleanalytics_trackoutbound', 1),
  );
  $form['tracking']['linktracking']['googleanalytics_trackmailto'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track clicks on mailto links'),
    '#default_value' => variable_get('googleanalytics_trackmailto', 1),
  );
  $form['tracking']['linktracking']['googleanalytics_trackfiles'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track downloads (clicks on file links) for the following extensions'),
    '#default_value' => variable_get('googleanalytics_trackfiles', 1),
  );
  $form['tracking']['linktracking']['googleanalytics_trackfiles_extensions'] = array(
    '#title' => t('List of download file extensions'),
    '#title_display' => 'invisible',
    '#type' => 'textfield',
    '#default_value' => variable_get('googleanalytics_trackfiles_extensions', GOOGLEANALYTICS_TRACKFILES_EXTENSIONS),
    '#description' => t('A file extension list separated by the | character that will be tracked as download when clicked. Regular expressions are supported. For example: !extensions', array('!extensions' => GOOGLEANALYTICS_TRACKFILES_EXTENSIONS)),
    '#maxlength' => 500,
    '#states' => array(
      'enabled' => array(
        ':input[name="googleanalytics_trackfiles"]' => array('checked' => TRUE),
      ),
      // Note: Form required marker is not visible as title is invisible.
      'required' => array(
        ':input[name="googleanalytics_trackfiles"]' => array('checked' => TRUE),
      ),
    ),
  );

  $colorbox_dependencies = '<div class="admin-requirements">';
  $colorbox_dependencies .= t('Requires: !module-list', array('!module-list' => (module_exists('colorbox') ? t('@module (<span class="admin-enabled">enabled</span>)', array('@module' => 'Colorbox')) : t('@module (<span class="admin-disabled">disabled</span>)', array('@module' => 'Colorbox')))));
  $colorbox_dependencies .= '</div>';

  $form['tracking']['linktracking']['googleanalytics_trackcolorbox'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track content in colorbox modal dialogs'),
    '#default_value' => variable_get('googleanalytics_trackcolorbox', 1),
    '#description' => t('Enable to track the content shown in colorbox modal windows.') . $colorbox_dependencies,
    '#disabled' => (module_exists('colorbox') ? FALSE : TRUE),
  );
  $form['tracking']['linktracking']['googleanalytics_tracklinkid'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track enhanced link attribution'),
    '#default_value' => variable_get('googleanalytics_tracklinkid', 0),
    '#description' => t('Enhanced Link Attribution improves the accuracy of your In-Page Analytics report by automatically differentiating between multiple links to the same URL on a single page by using link element IDs. <a href="@url">Enable enhanced link attribution</a> in the Admin UI of your Google Analytics account.', array('@url' => 'https://support.google.com/analytics/answer/2558867')),
  );
  $form['tracking']['linktracking']['googleanalytics_trackurlfragments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track changing URL fragments as pageviews'),
    '#default_value' => variable_get('googleanalytics_trackurlfragments', 0),
    '#description' => t('By default, the URL reported to Google Analytics will not include the "fragment identifier" (i.e. the portion of the URL beginning with a hash sign), and hash changes by themselves will not cause new pageviews to be reported. Checking this box will cause hash changes to be reported as pageviews (in modern browsers) and all pageview URLs to include the fragment where applicable.'),
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

  $form['tracking']['search_and_advertising'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search and Advertising'),
  );

  $site_search_dependencies = '<div class="admin-requirements">';
  $site_search_dependencies .= t('Requires: !module-list', array('!module-list' => (module_exists('search') ? t('@module (<span class="admin-enabled">enabled</span>)', array('@module' => 'Search')) : t('@module (<span class="admin-disabled">disabled</span>)', array('@module' => 'Search')))));
  $site_search_dependencies .= '</div>';

  $form['tracking']['search_and_advertising']['googleanalytics_site_search'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track internal search'),
    '#description' => t('If checked, internal search keywords are tracked. You must configure your Google account to use the internal query parameter <strong>search</strong>. For more information see <a href="@url">Setting Up Site Search for a Profile</a>.', array('@url' => 'https://support.google.com/analytics/answer/1012264')) . $site_search_dependencies,
    '#default_value' => variable_get('googleanalytics_site_search', FALSE),
    '#disabled' => (module_exists('search') ? FALSE : TRUE),
  );
  $form['tracking']['search_and_advertising']['googleanalytics_trackadsense'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track AdSense ads'),
    '#description' => t('If checked, your AdSense ads will be tracked in your Google Analytics account.'),
    '#default_value' => variable_get('googleanalytics_trackadsense', FALSE),
  );
  $form['tracking']['search_and_advertising']['googleanalytics_trackdoubleclick'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track display features'),
    '#description' => t('The display features plugin can be used to enable Display Advertising Features in Google Analytics, such as Remarketing, Demographics and Interest Reporting, and more. <a href="@displayfeatures">Learn more about Display Advertising Features in Google Analytics</a>. If you choose this option you will need to <a href="@privacy">update your privacy policy</a>.', array('@displayfeatures' => 'https://support.google.com/analytics/answer/3450482', '@privacy' => 'https://support.google.com/analytics/answer/2700409')),
    '#default_value' => variable_get('googleanalytics_trackdoubleclick', FALSE),
  );

  // Privacy specific configurations.
  $form['tracking']['privacy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Privacy'),
  );
  $form['tracking']['privacy']['googleanalytics_tracker_anonymizeip'] = array(
    '#type' => 'checkbox',
    '#title' => t('Anonymize visitors IP address'),
    '#description' => t('Tell Google Analytics to anonymize the information sent by the tracker objects by removing the last octet of the IP address prior to its storage. Note that this will slightly reduce the accuracy of geographic reporting. In some countries it is not allowed to collect personally identifying information for privacy reasons and this setting may help you to comply with the local laws.'),
    '#default_value' => variable_get('googleanalytics_tracker_anonymizeip', 1),
  );
  $form['tracking']['privacy']['googleanalytics_privacy_donottrack'] = array(
    '#type' => 'checkbox',
    '#title' => t('Universal web tracking opt-out'),
    '#description' => t('If enabled and your server receives the <a href="@donottrack">Do-Not-Track</a> header from the client browser, the Google Analytics module will not embed any tracking code into your site. Compliance with Do Not Track could be purely voluntary, enforced by industry self-regulation, or mandated by state or federal law. Please accept your visitors privacy. If they have opt-out from tracking and advertising, you should accept their personal decision. This feature is currently limited to logged in users and disabled page caching.', array('@donottrack' => 'http://donottrack.us/')),
    '#default_value' => variable_get('googleanalytics_privacy_donottrack', 1),
  );

  // Custom Dimensions.
  $form['googleanalytics_custom_dimension'] = array(
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#description' => t('You can set values for Google Analytics <a href="@custom_var_documentation">Custom Dimensions</a> here. You must have already configured your custom dimensions in the <a href="@setup_documentation">Google Analytics Management Interface</a>. You may use tokens. Global and user tokens are always available; on node pages, node tokens are also available. A dimension <em>value</em> is allowed to have a maximum length of 150 bytes. Expect longer values to get trimmed.', array('@custom_var_documentation' => 'https://developers.google.com/analytics/devguides/collection/analyticsjs/custom-dims-mets', '@setup_documentation' => 'https://support.google.com/analytics/answer/2709829')),
    '#theme' => 'googleanalytics_admin_custom_var_table',
    '#title' => t('Custom dimensions'),
    '#tree' => TRUE,
    '#type' => 'fieldset',
  );

  $googleanalytics_custom_dimension = variable_get('googleanalytics_custom_dimension', array());

  // Google Analytics supports up to 20 custom dimensions.
  for ($i = 1; $i <= 20; $i++) {
    $form['googleanalytics_custom_dimension']['indexes'][$i]['index'] = array(
      '#default_value' => $i,
      '#description' => t('Index number'),
      '#disabled' => TRUE,
      '#size' => 1,
      '#title' => t('Custom dimension index #@index', array('@index' => $i)),
      '#title_display' => 'invisible',
      '#type' => 'textfield',
    );
    $form['googleanalytics_custom_dimension']['indexes'][$i]['value'] = array(
      '#default_value' => isset($googleanalytics_custom_dimension[$i]['value']) ? $googleanalytics_custom_dimension[$i]['value'] : '',
      '#description' => t('The custom dimension value.'),
      '#maxlength' => 255,
      '#title' => t('Custom dimension value #@index', array('@index' => $i)),
      '#title_display' => 'invisible',
      '#type' => 'textfield',
      '#element_validate' => array('googleanalytics_token_element_validate'),
      '#token_types' => array('node'),
    );
    if (module_exists('token')) {
      $form['googleanalytics_custom_dimension']['indexes'][$i]['value']['#element_validate'][] = 'token_element_validate';
    }
  }

  $form['googleanalytics_custom_dimension']['googleanalytics_description'] = array(
    '#type' => 'item',
    '#description' => t('You can supplement Google Analytics\' basic IP address tracking of visitors by segmenting users based on custom dimensions. Section 7 of the <a href="@ga_tos">Google Analytics terms of service</a> requires that You will not (and will not allow any third party to) use the Service to track, collect or upload any data that personally identifies an individual (such as a name, userid, email address or billing information), or other data which can be reasonably linked to such information by Google. You will have and abide by an appropriate Privacy Policy and will comply with all applicable laws and regulations relating to the collection of information from Visitors. You must post a Privacy Policy and that Privacy Policy must provide notice of Your use of cookies that are used to collect traffic data, and You must not circumvent any privacy features (e.g., an opt-out) that are part of the Service.', array('@ga_tos' => 'http://www.google.com/analytics/terms/gb.html')),
  );
  if (module_exists('token')) {
    $form['googleanalytics_custom_dimension']['googleanalytics_token_tree'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array('node'),
      '#dialog' => TRUE,
    );
  }

  // Custom Metrics.
  $form['googleanalytics_custom_metric'] = array(
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#description' => t('You can add Google Analytics <a href="@custom_var_documentation">Custom Metrics</a> here. You must have already configured your custom metrics in the <a href="@setup_documentation">Google Analytics Management Interface</a>. You may use tokens. Global and user tokens are always available; on node pages, node tokens are also available.', array('@custom_var_documentation' => 'https://developers.google.com/analytics/devguides/collection/analyticsjs/custom-dims-mets', '@setup_documentation' => 'https://support.google.com/analytics/answer/2709829')),
    '#theme' => 'googleanalytics_admin_custom_var_table',
    '#title' => t('Custom metrics'),
    '#tree' => TRUE,
    '#type' => 'fieldset',
  );

  $googleanalytics_custom_metric = variable_get('googleanalytics_custom_metric', array());

  // Google Analytics supports up to 20 custom metrics.
  for ($i = 1; $i <= 20; $i++) {
    $form['googleanalytics_custom_metric']['indexes'][$i]['index'] = array(
      '#default_value' => $i,
      '#description' => t('Index number'),
      '#disabled' => TRUE,
      '#size' => 1,
      '#title' => t('Custom metric index #@index', array('@index' => $i)),
      '#title_display' => 'invisible',
      '#type' => 'textfield',
    );
    $form['googleanalytics_custom_metric']['indexes'][$i]['value'] = array(
      '#default_value' => isset($googleanalytics_custom_metric[$i]['value']) ? $googleanalytics_custom_metric[$i]['value'] : '',
      '#description' => t('The custom metric value.'),
      '#maxlength' => 255,
      '#title' => t('Custom metric value #@index', array('@index' => $i)),
      '#title_display' => 'invisible',
      '#type' => 'textfield',
      '#element_validate' => array('googleanalytics_token_element_validate'),
      '#token_types' => array('node'),
    );
    if (module_exists('token')) {
      $form['googleanalytics_custom_metric']['indexes'][$i]['value']['#element_validate'][] = 'token_element_validate';
    }
  }

  $form['googleanalytics_custom_metric']['googleanalytics_description'] = array(
    '#type' => 'item',
    '#description' => t('You can supplement Google Analytics\' basic IP address tracking of visitors by segmenting users based on custom metrics. Section 7 of the <a href="@ga_tos">Google Analytics terms of service</a> requires that You will not (and will not allow any third party to) use the Service to track, collect or upload any data that personally identifies an individual (such as a name, userid, email address or billing information), or other data which can be reasonably linked to such information by Google. You will have and abide by an appropriate Privacy Policy and will comply with all applicable laws and regulations relating to the collection of information from Visitors. You must post a Privacy Policy and that Privacy Policy must provide notice of Your use of cookies that are used to collect traffic data, and You must not circumvent any privacy features (e.g., an opt-out) that are part of the Service.', array('@ga_tos' => 'http://www.google.com/analytics/terms/gb.html')),
  );
  if (module_exists('token')) {
    $form['googleanalytics_custom_metric']['googleanalytics_token_tree'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array('node'),
      '#dialog' => TRUE,
    );
  }

  // Advanced feature configurations.
  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced']['googleanalytics_cache'] = array(
    '#type' => 'checkbox',
    '#title' => t('Locally cache tracking code file'),
    '#description' => t("If checked, the tracking code file is retrieved from Google Analytics and cached locally. It is updated daily from Google's servers to ensure updates to tracking code are reflected in the local copy. Do not activate this until after Google Analytics has confirmed that site tracking is working!"),
    '#default_value' => variable_get('googleanalytics_cache', 0),
  );

  // Allow for tracking of the originating node when viewing translation sets.
  if (module_exists('translation')) {
    $form['advanced']['googleanalytics_translation_set'] = array(
      '#type' => 'checkbox',
      '#title' => t('Track translation sets as one unit'),
      '#description' => t('When a node is part of a translation set, record statistics for the originating node instead. This allows for a translation set to be treated as a single unit.'),
      '#default_value' => variable_get('googleanalytics_translation_set', 0),
    );
  }

  $user_access_add_js_snippets = !user_access('add JS snippets for google analytics');
  $user_access_add_js_snippets_permission_warning = $user_access_add_js_snippets ? ' <em>' . t('This field has been disabled because you do not have sufficient permissions to edit it.') . '</em>' : '';
  $form['advanced']['codesnippet'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom JavaScript code'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('You can add custom Google Analytics <a href="@snippets">code snippets</a> here. These will be added every time tracking is in effect. Before you add your custom code, you should read the <a href="@ga_concepts_overview">Google Analytics Tracking Code - Functional Overview</a> and the <a href="@ga_js_api">Google Analytics Tracking API</a> documentation. <strong>Do not include the &lt;script&gt; tags</strong>, and always end your code with a semicolon (;).', array('@snippets' => 'http://drupal.org/node/248699', '@ga_concepts_overview' => 'https://developers.google.com/analytics/resources/concepts/gaConceptsTrackingOverview', '@ga_js_api' => 'https://developers.google.com/analytics/devguides/collection/analyticsjs/method-reference')),
  );
  $form['advanced']['codesnippet']['googleanalytics_codesnippet_create'] = array(
    '#type' => 'textarea',
    '#title' => t('Create only fields'),
    '#default_value' => variable_get('googleanalytics_codesnippet_create', ''),
    '#rows' => 5,
    '#description' => t('Enter one value per line, in the format name|value. Settings in this textarea will be added to <code>ga("create", "UA-XXXX-Y", {"name":"value"});</code>. For more information, read <a href="@url">create only fields</a> documentation in the Analytics.js field reference.', array('@url' => 'https://developers.google.com/analytics/devguides/collection/analyticsjs/field-reference#create')),
    '#element_validate' => array('googleanalytics_validate_create_field_values'),
  );
  $form['advanced']['codesnippet']['googleanalytics_codesnippet_before'] = array(
    '#type' => 'textarea',
    '#title' => t('Code snippet (before)'),
    '#default_value' => variable_get('googleanalytics_codesnippet_before', ''),
    '#disabled' => $user_access_add_js_snippets,
    '#rows' => 5,
    '#description' => t('Code in this textarea will be added <strong>before</strong> <code>ga("send", "pageview");</code>.') . $user_access_add_js_snippets_permission_warning,
  );
  $form['advanced']['codesnippet']['googleanalytics_codesnippet_after'] = array(
    '#type' => 'textarea',
    '#title' => t('Code snippet (after)'),
    '#default_value' => variable_get('googleanalytics_codesnippet_after', ''),
    '#disabled' => $user_access_add_js_snippets,
    '#rows' => 5,
    '#description' => t('Code in this textarea will be added <strong>after</strong> <code>ga("send", "pageview");</code>. This is useful if you\'d like to track a site in two accounts.') . $user_access_add_js_snippets_permission_warning,
  );

  $form['advanced']['googleanalytics_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable debugging'),
    '#description' => t('If checked, the Google Universal Analytics debugging script will be loaded. You should not enable your production site to use this version of the JavaScript. The analytics_debug.js script is larger than the analytics.js tracking code and it is not typically cached. Using it in your production site will slow down your site for all of your users. Again, this is only for your own testing purposes. Debug messages are printed to the <code>window.console</code> object.'),
    '#default_value' => variable_get('googleanalytics_debug', 0),
  );

  return system_settings_form($form);

}

