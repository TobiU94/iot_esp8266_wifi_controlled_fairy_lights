Here is a clear, developer-friendly guide designed to drop directly into your repository’s `README.md` file. It explains the purpose of Semantic Versioning, how the version numbers break down, and how developers should format commit messages to drive the automated pipeline.

---

```markdown
## 🏷️ Versioning & Release Conventions

This project follows **[Semantic Versioning (SemVer 2.0.0)](https://semver.org/)** for all firmware releases. Version numbers are automatically calculated and tagged during the CI/CD pipeline based on commit messages merged into `main`.

---

### Why Use Semantic Versioning?

1. **Predictable Updates:** Clearly communicates the impact of new firmware builds (whether a release is safe for background OTA updates or contains breaking API/hardware changes).
2. **Automated Releases:** Eliminates manual tagging and release creation. Merging code into `main` automatically triggers a version bump, generates release notes, and attaches compiled binaries.
3. **Reliable OTA Alignment:** Ensures the ESP8266 board can cleanly compare its running firmware version (`0.1.0`) against the update server (`version.txt`) without string mismatches or prefix issues.

---

### Understanding SemVer Structure (`MAJOR.MINOR.PATCH`)

Given a version number **`X.Y.Z`** (e.g., `v1.2.4`):

* **MAJOR (`X`)**: Incremented for **breaking changes** or incompatible architecture/API shifts (e.g., changes to internal flash structures, network protocols, or pinouts that break backwards compatibility).
* **MINOR (`Y`)**: Incremented for **new features** added in a backwards-compatible manner (e.g., adding a new LED animation mode or a new HTTP endpoint).
* **PATCH (`Z`)**: Incremented for **backwards-compatible bug fixes** and performance tweaks (e.g., fixing a WiFi reconnection loop or memory leak).

---

### How to Control Version Bumps via Commit Messages

Our automated pipeline uses `mathieudutour/github-tag-action` to parse pull requests and commit messages when merging to `main`. 

By default, any PR merged to `main` without special tags will trigger a **`PATCH`** bump. To trigger a **`MINOR`** or **`MAJOR`** bump, include the corresponding hashtag anywhere in your commit message or PR title:

| Version Bump | Commit Keyword / Tag | Example Commit Message | Next Version (from `0.1.0`) |
| :--- | :--- | :--- | :--- |
| **PATCH** *(Default)* | *(None or `#patch`)* | `fix: resolve softAP timeout issue` | `0.1.1` |
| **MINOR** | `#minor` | `feat: add support for MQTT telemetry #minor` | `0.2.0` |
| **MAJOR** | `#major` | `refactor!: rewrite REST API structure #major` | `1.0.0` |

> 💡 **Tip:** When creating a Pull Request from `development` into `main`, add the hashtag (`#minor` or `#major`) directly to the **PR Title** or the final **Squash & Merge commit description**.

---

### Branch & Versioning Flow

* **Feature Branches (`feature/*`):** Continuous integration builds binaries tagged with a `-dev.<run_number>` suffix (e.g., `0.1.1-dev.42`) for testing. No official release is created.
* **Development Branch (`development`):** Consolidates features and tests integration before tagging.
* **Main Branch (`main`):** Official releases only. Merges to `main` auto-generate the official Git Tag (e.g., `v0.1.1`), `version.txt`, and GitHub Release assets.

```