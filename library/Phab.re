open Lwt;
open Cohttp;
open Cohttp_lwt_unix;

let run = () => {
  let%Async (out, err, res) = Fetch.runCommand("git", [|"git", "log", "HEAD", "HEAD@{upstream}"|]);
  print_endline("Result!");
  print_endline(out);
  print_endline(err);
  Async.resolve();
}

let run = () => Lwt_main.run(run());
