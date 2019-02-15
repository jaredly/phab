
let readFd = (descr) => {
  let buffer_size = 100;
  let bytes = Bytes.create(buffer_size);
  let buffer = Buffer.create(buffer_size);
  let rec copy_loop = () => {
    let%Async count = Lwt_unix.read(descr, bytes, 0, buffer_size);
    switch (count) {
    | 0 => Lwt.return()
    | r =>
      Buffer.add_subbytes(buffer, bytes, 0, r);
      let%Async () = copy_loop();
      Lwt.return()
    };
  };
  let%Async () = copy_loop();
  Async.resolve(Buffer.contents(buffer))
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
  let%Async out = readFd(stdout);
  let%Async err = readFd(stderr);
  let%Async result = process;
  Async.resolve((out, err, result))
};
