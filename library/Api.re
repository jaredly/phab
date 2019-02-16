
module Json = RexJson.Json;

let getAuth = () => {
  let home = Sys.getenv("HOME");
  let fpath = Filename.concat(home, ".arcrc");
  let%Async data = Fs.readFile(fpath);
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

let getUpstream = (branch) => {
  let%Async (out, _, _) = Cmd.run([|"git", "rev-parse", "--abbrev-ref", branch ++ "@{upstream}"|]);
  Async.resolve(Base__String.strip(out))
};
let phabRx = Str.regexp(" *Differential Revision:.*/D\\([0-9]+\\) *");
let getPhab = (branch) => {
  let%Async (out, _, result) = Cmd.run([|"git", "log", branch ++ "@{upstream}.." ++ branch|]);
  // print_endline(branch)
  switch result {
    | WEXITED(0) =>
      let lines = String.split_on_char('\n', out);
      let rec find = lines => switch lines {
        | [] => None
        | [line, ...rest] =>
          if (Str.string_match(phabRx, line, 0)) {
            let id = Str.matched_group(1, line);
            Some(int_of_string(id))
          } else {
            find(rest)
          }
      };
      find(lines)
    | _ =>
    print_endline("bad exit");
    None
  } |> Async.resolve
};

let kwargs = items => Uri.encoded_of_query(items->Belt.List.map(((a, b)) => (a, [b])));

let call = (endpoint, args) => {
  let%Async (hostname, token, base) = auth;
  let url = hostname ++ endpoint ++ "?" ++ Fetch.kwargs([("api.token", token), ...args]);
  let%Async response = Fetch.fetch(url);
  let%Async json = response->Fetch.json;
  let result = json |> Json.get("result");
  switch result {
    | Some(result) => Async.resolve(result);
    | None => failwith("api error")
  }
};

let getPhids = (ids) => {
  let%Async data =
    call(
      "differential.revision.search",
      ids->Belt.List.mapWithIndex((index, id) =>
        (
          "constraints[ids][" ++ string_of_int(index) ++ "]",
          string_of_int(id),
        )
      ),
    );
  let phids = {
    open Json.Infix;
    let%Lets.Opt.Force data = data |> Json.get("data") |?> Json.array;
    data->Belt.List.reduce(Belt.Map.Int.empty, (map, json) => switch ({
      module O = Lets.Opt;
      let%O id = json |> Json.get("id") |?> Json.number |?>> int_of_float;
      let%O phid = json |> Json.get("phid") |?> Json.string;
      Some((phid, id))
    }) {
      | None => map
      | Some((phid, id)) => map->Belt.Map.Int.set(id, phid)
    })
  };
  Async.resolve(phids)
};

let setParent = (child, parent) => {
  let%Async data = call("differential.revision.edit", [
    ("transactions[0][type]", "parents.set"),
    ("transactions[0][value][0]", parent),
    ("objectIdentifier", child)
  ]);
  Async.resolve()
}

