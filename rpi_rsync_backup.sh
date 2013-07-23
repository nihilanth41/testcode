#!/bin/bash
_BACKUP_DIR="/mnt/ext/backups/rpi_rsync_backup/"
_now=$(date +"%m_%d_%Y")
rsync -aEv --delete-during --exclude-from=/mnt/ext/backups/rpi_rsync_exclude.txt / $_BACKUP_DIR > /mnt/ext/backups/rpi_rsync_backup_$_now.log
