# Android Dear ImGui Demo — cloud APK build

This is a standalone Android app using Dear ImGui and synthetic test objects. It does not inject into Standoff 2 or access another app's memory/game state.

## Build the APK using GitHub Actions (phone-only workflow)

1. Create a new repository on GitHub.
2. Upload the contents of this project folder to the repository root. Keep the `.github/workflows/android.yml` file; if the mobile upload page hides dot-folders, create that workflow file directly in GitHub using **Add file → Create new file** and the path `.github/workflows/android.yml`.
3. Commit the files to `main` (or `master`).
4. Open the repository's **Actions** tab and choose **Build Android APK**. If the workflow is not visible yet, open **Actions** and enable workflows for the repository.
5. Select **Run workflow** if needed. Wait for the run to finish.
6. Open the successful run and download the `Dear-ImGui-Demo-debug-apk` artifact. Unzip the artifact to get the APK, then install it on your phone. Android may ask you to allow installation from your browser/file manager.

## Local build requirements

- JDK 17
- Android SDK platform 35
- Android NDK 27.2.12479018
- CMake 3.22.1
- Gradle 8.9

The GitHub Actions workflow installs these tools automatically.

## Notes

The APK is a debug build for testing, not a Play Store release. The demo renders only synthetic placeholder objects inside its own app surface. It is not a floating overlay over Standoff 2, not an injector, and not connected to live game entities.
