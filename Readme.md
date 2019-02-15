# phab

A tool for managing phabricator revisions from the command line.

Currently has one job: set up the parent/child relationship between the current diff & the one on the upstream branch.

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running Binary:

After building the project, you can run the main binary that is produced.

```
esy x phab.exe
```
