
let readFd = (descr) => {
  let buffer_size = 8192;
  let buffer = Bytes.create(buffer_size);
  let rec copy_loop = (offset) => {
    print_endline("Here " ++ string_of_int(offset));
    let%Async count = Lwt_unix.read(descr, buffer, 0, buffer_size);
    switch (count) {
    | 0 => Lwt.return()
    | r =>
      let%Async () = copy_loop(offset + r);
      Lwt.return()
    };
  };
  let%Async () = copy_loop(0);
  Async.resolve(Bytes.to_string(buffer))
};

let readFile = (path) => {
  let%Async descr = Lwt_unix.openfile(path, [Unix.O_RDONLY], 0o640);
  readFd(descr)
}

let getAuth = () => {
  let fpath = Filename.concat("", ".arcrc");
  let%Async data = readFile(fpath);
  let json = Json.parse(data);
  open Json.Infix;
  let base = json |> Json.get("config") |?> Json.get("default") |?> Json.string |! "No config.default";
  let hosts = json |> Json.get("hosts") |?> Json.obj |? [];
  switch hosts {
    | [] => failwith("No hosts in .arcrc")
    | [(hostname, obj), ..._] => switch (obj |> Json.get("token") |?> Json.string) {
      | None => failwith("No token for host in .arcrc")
      | Some(token) => Async.resolve((hostname, token, base))
    }
  }
};

let auth = getAuth();

// let kwargs = items => String.concat("&", items->Belt.List.map(((k, v)) => k ++ "=" ++ EncodeURIComponent.encode(v)));

let runCommand = (command, args) => {
  let (stderr, stderr_i) = Lwt_unix.pipe_in();
  let (stdout, stdout_i) = Lwt_unix.pipe_in();
  let process = Lwt_process.exec(~stdout=`FD_move(stdout_i), ~stderr=`FD_move(stderr_i), (command, args));
  let%Async result = process;
  print_endline("A");
  let%Async out = readFd(stdout);
  print_endline("B");
  let%Async err = readFd(stderr);
  print_endline("C");
  Async.resolve((out, err, result))
};
