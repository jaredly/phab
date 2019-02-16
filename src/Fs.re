
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
