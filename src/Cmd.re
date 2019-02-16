
let run = (args) => {
  let (stderr, stderr_i) = Lwt_unix.pipe_in();
  let (stdout, stdout_i) = Lwt_unix.pipe_in();
  let process = Lwt_process.exec(~stdout=`FD_move(stdout_i), ~stderr=`FD_move(stderr_i), ("", args));
  let%Async out = Fs.readFd(stdout);
  let%Async err = Fs.readFd(stderr);
  let%Async result = process;
  Async.resolve((out, err, result))
};
