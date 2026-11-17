void main(ScriptEngine@ e) {
    
    // Retrieve the profile category named "Platform"
    ProfileCategaory@ platformProfile = e.getProfileByName("Platform");

     // Create a new entity named "Entity" using the retrieved Platform profile
    // and add it to the platform. Store the returned Platform object handle.
    Platform@ myPlatform = e.addEntityToPlatform(platformProfile, "Entity");
}
