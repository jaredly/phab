open Lwt;
open Cohttp;
open Cohttp_lwt_unix;

let reparent = () => {
  let%Async upStream = Api.getUpstream("HEAD");
  if (upStream == "master") {
    print_endline("Upstream is master. Aborting.");
    exit(1)
  } else {
    let%Async phab = Api.getPhab("HEAD");
    let%Async upPhab = Api.getPhab(upStream);
    switch (phab, upPhab) {
      | (Some(phab), Some(upPhab)) when phab == upPhab =>
        print_endline("This branch doesn't have a revision yet. Aborting");
        exit(1)
      | (Some(phab), Some(upPhab)) =>
        Printf.printf("Mine: D%d - Theirs: D%d\n", phab, upPhab)
        let%Async phids = Api.getPhids([phab, upPhab]);
        switch (phids->Belt.Map.Int.get(phab), phids->Belt.Map.Int.get(upPhab)) {
          | (Some(phab), Some(upPhab)) =>
            print_endline("Adding parent relationship");
            Api.setParent(phab, upPhab);
          | _ => failwith("Not found revisions")
        }
        // Async.resolve();
      | _ => print_endline("Failed to find revision IDs")
        Async.resolve();
    }
  }
}

let parentAll = () => {
  let rec loop = (child, branch) => {
    let%Async phab = Api.getPhab(branch);
    let%Async () = switch (child, phab) {
      | (Some(child), Some(parent)) =>
        let%Async phids = Api.getPhids([child, parent]);
        switch (phids->Belt.Map.Int.get(child), phids->Belt.Map.Int.get(parent)) {
          | (Some(child), Some(parent)) =>
          print_endline("Parenting " ++ branch);
          Api.setParent(child, parent)
          | _ =>
            print_endline("Unable to resolve IDs")
            Async.resolve()
        }
      | _ =>
        print_endline("Not parenting " ++ branch);
        Async.resolve()
    };
    let text = switch (phab) {
      | None => "!!"
      | Some(id) => string_of_int(id)
    };
    print_endline(text ++ ": " ++ branch)
    let%Async up = Api.getUpstream(branch);
    if (up == "master") {
      Async.resolve()
    } else if (up == branch) {
      Async.resolve()
    } else {
      loop(phab, up)
    }
  };
  loop(None, "HEAD")
};

let showBranches = () => {
  let rec loop = (branch) => {
    let%Async phab = Api.getPhab(branch);
    let text = switch (phab) {
      | None => "!!"
      | Some(id) => string_of_int(id)
    };
    print_endline(text ++ ": " ++ branch)
    let%Async up = Api.getUpstream(branch);
      // print_endline("Up: " ++ up);
    if (up == "master") {
      Async.resolve()
    } else if (up == branch) {
      Async.resolve()
    } else {
      loop(up)
    }
  };
  loop("HEAD")
};

let checkStatus = () => {
  Async.resolve()
};

// let run = () => Lwt_main.run(run());
