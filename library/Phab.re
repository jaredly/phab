open Lwt;
open Cohttp;
open Cohttp_lwt_unix;

let run = () => {
  let%Async upStream = Fetch.getUpstream();
  if (upStream == "master") {
    print_endline("Upstream is master. Aborting.");
    exit(1)
  } else {
    let%Async phab = Fetch.getPhab("HEAD");
    let%Async upPhab = Fetch.getPhab(upStream);
    switch (phab, upPhab) {
      | (Some(phab), Some(upPhab)) when phab == upPhab =>
        print_endline("This branch doesn't have a revision yet. Aborting");
        exit(1)
      | (Some(phab), Some(upPhab)) =>
        Printf.printf("Mine: D%d - Theirs: D%d\n", phab, upPhab)
        let%Async phids = Fetch.getPhids([phab, upPhab]);
        switch (phids->Belt.Map.Int.get(phab), phids->Belt.Map.Int.get(upPhab)) {
          | (Some(phab), Some(upPhab)) =>
            print_endline("Adding parent relationship");
            Fetch.setParent(phab, upPhab);
          | _ => failwith("Not found revisions")
        }
        // Async.resolve();
      | _ => print_endline("Failed to find revision IDs")
        Async.resolve();
    }
  }
}

let run = () => Lwt_main.run(run());
