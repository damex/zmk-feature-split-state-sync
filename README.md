# zmk-feature-split-state-sync

Central-side ZMK feature. Watches a peer's input stream and pushes a list of
bindings to it once per reconnect, detected as the first input after >10 s of
silence. For transports without a connect event (ESB, custom radio).

## Use

Add to your `config/west.yml`:
```yaml
  remotes:
    - name: damex
      url-base: https://github.com/damex
  projects:
    - name: zmk-feature-split-state-sync
      remote: damex
      revision: v0.1.0
      import: true
```

Declare on the central side, point it at the peer's input-split device:
```dts
/ {
    mouse_state: mouse_state {
        compatible = "zmk,split-state-sync";
        input = <&mouse_split>;
        source = <0>;
        bindings
            = <&some_config KEY_A VALUE_A>
            , <&some_config KEY_B VALUE_B>
            , <&other_config KEY_X VALUE_X>;
    };
};
```

Bindings should target the peer via `EVENT_SOURCE` locality so the call
routes across the split.

## Limitations

- Reconnect = input silence > 10 s. A peer that idles longer than that and
  then sends a single packet looks identical to a cold-boot. Fine when the
  bindings are idempotent.
- Single-peer central only — `source` is one fixed id.
- Bindings are compile-time.
