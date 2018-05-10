# General functions

import os


def get_slack_webhook_url():
    return os.environ["SLACK_WEBHOOK_URL"]
