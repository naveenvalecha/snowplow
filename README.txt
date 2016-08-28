This module provides integration with the Snowplow event analtyics pipeleine.

The core module provides basic configuration and API integration. Features and
site functionality are provided by a set of submodules that depend upon the core
"mailchimp" module. These are in the "modules" subdirectory: See their
respective README's for more details.

## Features

## Installation Notes
  * You need to have a MailChimp API Key.
  * You need to have at least one list created in MailChimp to use the
    mailchimp_lists module.
  * If you use a drush make workflow, see the example drush makefile:
    mailchimp.make.example.

  * The MailChimp PHP library must exist in your libraries directory.

    For module version 7.x-4.x:

      - Download Composer if you don't already have it installed:
        https://getcomposer.org/download/

      - Download version 1.0 of the v3 API library:
        https://github.com/thinkshout/mailchimp-api-php/releases

      - Extract the library archive to libraries/mailchimp

      - Ensure the directory structure looks like this:

        - libraries/
          - mailchimp/
            - src/
              - Mailchimp.php
              - MailchimpAPIException.php
              - MailchimpCampaigns.php
              - MailchimpLists.php
              - MailchimpReports.php
              - MailchimpTemplates.php
            - composer.json
            - README.md

      - In the mailchimp library directory, run:
        composer install

    For module version 7.x-2.x and 7.x-3.x:

      - Download version 2.0.6 of the v2 API library:
        https://bitbucket.org/mailchimp/mailchimp-api-php/downloads

      - Extract the library archive to libraries/mailchimp

      - Ensure the directory structure looks like this:

        - libraries/
          - snowplow/
            - docs/
            - src/
              - Mailchimp.php
              - Mailchimp/
            - README.md
            - composer.json

## Configuration

## Submodules

