let kwargs = items => Uri.encoded_of_query(items->Belt.List.map(((a, b)) => (a, [b])));

type options = {
    body: Cohttp_lwt.Body.t,
    headers: list((string, string)),
    method: Cohttp.Code.meth,
};

let defaultOptions = {
  body: `Empty,
  method: `GET,
  headers: [],
};

let opts = (~body=`Empty, ~method=`GET, ~headers=[], ()) => {body, method, headers};

let text = ((_, body)) => Cohttp_lwt.Body.to_string(body);
let json = (response) => {
  let%Async body = text(response);
  Async.resolve(RexJson.Json.parse(body));
};

let fetch = (~options as {method, body, headers}=defaultOptions, url) => {
  let headers = Cohttp.Header.of_list(headers);
  Cohttp_lwt_unix.Client.call(~headers, ~body, method, Uri.of_string(url))
};