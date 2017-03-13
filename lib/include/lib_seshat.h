  /** *  Registers a service with Seshat. * *
@service the service name to register with Seshat (example: "webpa") *
@url     the url to reach the service at (example: "tcp://127.0.0.1:6543") * *
@return 0 if successful, error otherwise (Use ERRNO values & list them here)
*/
int seshat_register(const char *service, const char *url);

/** *  Discovers how to reach a service. * *
@service the service to find the url for * *
@return the url of the service if available, NULL otherwise. *
Use & set stderr for what the error is. */
char* seshat_discover(const char *service);
