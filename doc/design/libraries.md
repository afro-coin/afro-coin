# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libafrocoin_cli*         | RPC client functionality used by *afrocoin-cli* executable |
| *libafrocoin_common*      | Home for common functionality shared by different executables and libraries. Similar to *libafrocoin_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libafrocoin_consensus*   | Consensus functionality used by *libafrocoin_node* and *libafrocoin_wallet*. |
| *libafrocoin_crypto*      | Hardware-optimized functions for data encryption, hashing, message authentication, and key derivation. |
| *libafrocoin_kernel*      | Consensus engine and support library used for validation by *libafrocoin_node*. |
| *libafrocoinqt*           | GUI functionality used by *afrocoin-qt* and *afrocoin-gui* executables. |
| *libafrocoin_ipc*         | IPC functionality used by *afrocoin-node*, *afrocoin-wallet*, *afrocoin-gui* executables to communicate when [`-DWITH_MULTIPROCESS=ON`](multiprocess.md) is used. |
| *libafrocoin_node*        | P2P and RPC server functionality used by *afrocoind* and *afrocoin-qt* executables. |
| *libafrocoin_util*        | Home for common functionality shared by different executables and libraries. Similar to *libafrocoin_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libafrocoin_wallet*      | Wallet functionality used by *afrocoind* and *afrocoin-wallet* executables. |
| *libafrocoin_wallet_tool* | Lower-level wallet functionality used by *afrocoin-wallet* executable. |
| *libafrocoin_zmq*         | [ZeroMQ](../zmq.md) functionality used by *afrocoind* and *afrocoin-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. An exception is *libafrocoin_kernel*, which, at some future point, will have a documented external interface.

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`add_library(afrocoin_* ...)`](../../src/CMakeLists.txt) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libafrocoin_node* code lives in `src/node/` in the `node::` namespace
  - *libafrocoin_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libafrocoin_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libafrocoin_util* code lives in `src/util/` in the `util::` namespace
  - *libafrocoin_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

afrocoin-cli[afrocoin-cli]-->libafrocoin_cli;

afrocoind[afrocoind]-->libafrocoin_node;
afrocoind[afrocoind]-->libafrocoin_wallet;

afrocoin-qt[afrocoin-qt]-->libafrocoin_node;
afrocoin-qt[afrocoin-qt]-->libafrocoinqt;
afrocoin-qt[afrocoin-qt]-->libafrocoin_wallet;

afrocoin-wallet[afrocoin-wallet]-->libafrocoin_wallet;
afrocoin-wallet[afrocoin-wallet]-->libafrocoin_wallet_tool;

libafrocoin_cli-->libafrocoin_util;
libafrocoin_cli-->libafrocoin_common;

libafrocoin_consensus-->libafrocoin_crypto;

libafrocoin_common-->libafrocoin_consensus;
libafrocoin_common-->libafrocoin_crypto;
libafrocoin_common-->libafrocoin_util;

libafrocoin_kernel-->libafrocoin_consensus;
libafrocoin_kernel-->libafrocoin_crypto;
libafrocoin_kernel-->libafrocoin_util;

libafrocoin_node-->libafrocoin_consensus;
libafrocoin_node-->libafrocoin_crypto;
libafrocoin_node-->libafrocoin_kernel;
libafrocoin_node-->libafrocoin_common;
libafrocoin_node-->libafrocoin_util;

libafrocoinqt-->libafrocoin_common;
libafrocoinqt-->libafrocoin_util;

libafrocoin_util-->libafrocoin_crypto;

libafrocoin_wallet-->libafrocoin_common;
libafrocoin_wallet-->libafrocoin_crypto;
libafrocoin_wallet-->libafrocoin_util;

libafrocoin_wallet_tool-->libafrocoin_wallet;
libafrocoin_wallet_tool-->libafrocoin_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class afrocoin-qt,afrocoind,afrocoin-cli,afrocoin-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Crypto* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus, crypto, and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libafrocoin_wallet* and *libafrocoin_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libafrocoin_crypto* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libafrocoin_consensus* should only depend on *libafrocoin_crypto*, and all other libraries besides *libafrocoin_crypto* should be allowed to depend on it.

- *libafrocoin_util* should be a standalone dependency that any library can depend on, and it should not depend on other libraries except *libafrocoin_crypto*. It provides basic utilities that fill in gaps in the C++ standard library and provide lightweight abstractions over platform-specific features. Since the util library is distributed with the kernel and is usable by kernel applications, it shouldn't contain functions that external code shouldn't call, like higher level code targeted at the node or wallet. (*libafrocoin_common* is a better place for higher level code, or code that is meant to be used by internal applications only.)

- *libafrocoin_common* is a home for miscellaneous shared code used by different Afrocoin Core applications. It should not depend on anything other than *libafrocoin_util*, *libafrocoin_consensus*, and *libafrocoin_crypto*.

- *libafrocoin_kernel* should only depend on *libafrocoin_util*, *libafrocoin_consensus*, and *libafrocoin_crypto*.

- The only thing that should depend on *libafrocoin_kernel* internally should be *libafrocoin_node*. GUI and wallet libraries *libafrocoinqt* and *libafrocoin_wallet* in particular should not depend on *libafrocoin_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libafrocoin_consensus*, *libafrocoin_common*, *libafrocoin_crypto*, and *libafrocoin_util*, instead of *libafrocoin_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libafrocoinqt*, *libafrocoin_node*, *libafrocoin_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](../../src/interfaces/) abstract interfaces.

## Work in progress

- Validation code is moving from *libafrocoin_node* to *libafrocoin_kernel* as part of [The libafrocoinkernel Project #27587](https://github.com/afrocoin/afrocoin/issues/27587)
