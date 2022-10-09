# OSC Lab repo

A good practice is to make a clone of this repo for every lab. This way you have a folder on your hard drive with the code for every lab.
For lab 1 and all the next labs, you could do :
```bash
git clone <git@yourrepoURL> clab1gcc
cd clab1gcc
git switch clab1gcc
```

The last command in the clab1gcc folder `git switch clab1gcc` will switch to the clab1gcc branch in the repository.

When some parts of the exercise work as expected, don't forget to commit and push your code to the gitlab server:
```bash
# view what files are altered
git status

# if needed add files to the repo
git add main.c

# commit all new and updated files (write meaningful comment for each commit)
git commit

# push the updates to gitlab.groept.be
git push
```

## note
Every branch contains a `.gitignore` file that instructs git to automatically ignore `.a`, `.o`, `.so` files. Most of these are compiled files and don't belong in a repository. Besides these, `.zip` files probably include files that are already in your repo, so these are also ignored.

A good habit is to put all compiled files in a `./build` folder (e.g. check the makefile in clab3lists). This folder is also included in `.gitignore`, this way no compiled files will go into your repository.

You can change/update this `.gitignore` file in every branch, so it fits the needs for your lab assignment.
