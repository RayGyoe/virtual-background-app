// Copyright 2019 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MEDIAPIPE_ANDROID_UTIL_ASSET_MANAGER_UTIL_H_
#define MEDIAPIPE_ANDROID_UTIL_ASSET_MANAGER_UTIL_H_

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>

#include "mediapipe/framework/port/canonical_errors.h"
#include "mediapipe/framework/port/singleton.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/statusor.h"

namespace mediapipe {

// Thin wrapper over AAssetManager provided by JNI. This class is meant to be
// used as a singleton.
// Usage: Call InitializeFromActivity from a JNI function that has access to the
// Java activity in the Android application. This initializes the asset manager
// and now files bundled in the assets folder can be read using ReadFile().
class AssetManager {
 public:
  AssetManager(const AssetManager&) = delete;
  AssetManager& operator=(const AssetManager&) = delete;

  // Returns the asset manager if it has been set by a call to
  // InitializeFromActivity, otherwise returns nullptr.
  AAssetManager* GetAssetManager();
  // Returns true if AAssetManager was successfully initialized.
  bool InitializeFromAssetManager(JNIEnv* env, jobject local_asset_manager,
                                  const std::string& cache_dir_path);

  // Returns true if AAssetManager was successfully initialized.
  // cache_dir_path should be set to activity.getCacheDir().getAbsolutePath().
  // We could get it from the activity, but we have the Java layer pass it
  // directly for convenience.
  bool InitializeFromActivity(JNIEnv* env, jobject activity,
                              const std::string& cache_dir_path);

  // Returns true if AAssetManager was successfully initialized.
  ABSL_DEPRECATED("Use InitializeFromActivity instead.")
  bool InitializeFromAssetManager(JNIEnv* env, jobject local_asset_manager);

  // Returns true if AAssetManager was successfully initialized.
  // cache_dir_path should be set to context.getCacheDir().getAbsolutePath().
  // We could get it from the context, but we have the Java layer pass it
  // directly for convenience.
  bool InitializeFromContext(JNIEnv* env, jobject context,
                             const std::string& cache_dir_path);

  // Checks if a file exists. Returns true on success, false otherwise.
  bool FileExists(const std::string& filename);

  // Reads a file into raw_bytes. Returns true on success, false otherwise.
  bool ReadFile(const std::string& filename, std::vector<uint8_t>* raw_bytes);

  // Returns the open file descriptor from an Android content URI, the caller
  // is responsible to close the file descriptor.
  ::mediapipe::StatusOr<int> OpenContentUri(const std::string& content_uri);

  // Returns the path to the Android cache directory. Will be empty if
  // InitializeFromActivity has not been called.
  const std::string& GetCacheDirPath();

  // Caches the contents of the given asset as a file, and returns a path to
  // that file. This can be used to pass an asset to APIs that require a path
  // to a filesystem file.
  ::mediapipe::StatusOr<std::string> CachedFileFromAsset(
      const std::string& asset_path);

 private:
  // Private constructor since this class is meant to be a singleton.
  AssetManager() = default;

  // Pointer to asset manager from JNI.
  AAssetManager* asset_manager_ = nullptr;

  // The context from which assets should be loaded.
  jobject context_;

  // Pointer to the JVM, used to get the JNIEnv on background threads.
  JavaVM* jvm_;

  // Path to the Android cache directory for our context.
  std::string cache_dir_path_;

  friend class Singleton<AssetManager>;
};

}  // namespace mediapipe

#endif  // __ANDROID__

#endif  // MEDIAPIPE_ANDROID_UTIL_ASSET_MANAGER_UTIL_H_
