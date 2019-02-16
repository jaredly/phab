switch (Sys.argv) {
| [|_, "parent"|] => Lwt_main.run(Phab.reparent())
| [|_, "parent-all"|] => Lwt_main.run(Phab.parentAll())
| [|_, "s" | "st" | "status"|] => Lwt_main.run(Phab.checkStatus())
| [|_, "b" | "branches" |] => Lwt_main.run(Phab.showBranches())
| _ => print_endline({|
## Phab - a cli tool for handling phabricator diffs

Commans:
- parent -- set up the parent/child relationship between this diff and that of the upstream branch
- status | st | s -- check the status of the current diff
- branches | b -- show the phab ids of all branches back to master
|})
};