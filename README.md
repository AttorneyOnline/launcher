# Launcher

<p align="center">
  <img alt="Update available screenshot" src="screenshots/update-available.png">
</p>

This is the official Attorney Online launcher, which can also be customized for
custom clients or even any other piece of interesting software.

## To-do

- Make button to delete installation
- Delete downloaded files after extraction is complete

## Guide for server owners

The game is split into two packages: `program` and `assets`. Each package has a
different manifest and thus a different set of versions. This is convenient if
you wish to offer an up-to-date version of your own server assets, but still
wish to allow players to remain up-to-date with the official client.

### Manifest format

The manifest is written in JavaScript Object Notation (JSON), which is
significantly easier to read and write than YAML (a format you may be
familiarized with from tsuserver).

Please refer to `spec.ts` for a specification, and see the `manifests` folder
for examples.

A manifest contains a list of versions; the first version on the list is the
most recent version.

#### Versions

Each version contains the version name, the name of the previous version, the
executable name (if this version contains a full download), and a list of tasks
to be performed for a full download and/or an update.

A version may not contain a full download. In this case, the launcher will find
the most recent full download available (by going down `prev`).

A version may not contain an update. In this case, all players will be required
to perform the full download.

#### Tasks

A full or update installation operation is simply a list of tasks to be
performed. You may delete files or directories before or after performing a
download, or even display a message.

Download URLs must be direct links.

### Deploying new versions

#### Client

Deploying a new client version will require building for the necessary
platforms (`winnt_i386`, `linux_x86_64`) and adding version entries to the
respective manifest files. Ensure that the `prev` entry is set to the previous
version.

Do not distribute `base/config.ini` or other configuration files in client
downloads, as this will consistently overwrite player-set settings. Instead,
distribute a `base/config.sample.ini` file, which explains all of the possible
settings and their default values and can be renamed to `config.ini` by the
player.

#### Assets

You should package your assets with the 7z format using LZMA2 solid
compression, which should yield an extremely high compression ratio compared
to ZIP.

It is recommended to segment very large downloads into multiple archives. If
an error occurs, the updater can detect which archives have already been
downloaded, but only if the hash for each archive has been included in
the download.

#### Botched deploys

If a mishap (botch) occurs in either an asset or client version, **DO NOT ROLL
BACK - ROLL FORWARD**. Rolling back will cause the launcher to perform a full
download for the old version again for everyone. Instead, roll forward by doing
the following:

 - Create a new version of the same name as the old version.
 - Set the new version's update task to revert the damage done by the botched
   version.
 - Set the new version's full download task list to be the same as the old
   version.
 - Set the full and update task list of the botched version to be empty.

Clients that did not update during the botch period will not be asked to
update.

## License

Copyright (c) 2018 oldmud0 (github.com/oldmud0)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
