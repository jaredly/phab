# phab


[![CircleCI](https://circleci.com/gh/yourgithubhandle/phab/tree/master.svg?style=svg)](https://circleci.com/gh/yourgithubhandle/phab/tree/master)


**Contains the following libraries and executables:**

```
phab@0.0.0
│
├─test/
│   name:    TestPhab.exe
│   main:    TestPhab
│   require: phab.lib
│
├─library/
│   library name: phab.lib
│   namespace:    Phab
│   require:
│
└─executable/
    name:    PhabApp.exe
    main:    PhabApp
    require: phab.lib
```

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
esy x PhabApp.exe 
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
