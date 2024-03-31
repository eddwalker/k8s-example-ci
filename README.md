Minimalistic example for creating Docker images for offline scenarios Kubernetes clusters with the Pull model.
While the primary goal of using the Pull model is Canary deployment,
the goal for offline scenarios is also reproducible and predictable Kubernetes clusters.

Note: this repo depends on the services deployed from gitlab-settings repo. See gitlab-settings/README.md for more details.
Note: This repo is about a CI flow only, so there are no parts that may run on the k8s cluster, like new images detection or deploy to Dev cluster custom namespace.

How it works:

1. The programmer pushes code to a new branch of the app repository on Gitlab.
2. Gitlab Pipeline immediately:
  - creates a reproducible Docker image with Kaniko
  - puts this image in the local Gitlab Docker Registry
  - creates a temporary namespace in the Dev cluster and deploys the new app image in (not implemented yet)
  - writes out the image path and and 'commit tag' in the Registry, namespace in the Dev cluster and web URL to access the app in Dev cluster
3. The programmer can create a merge request to the main branch if the app looks OK in the Dev cluster and QA agrees with it.
4. The lead or other responsible person can accept the merge request to the main branch, and this will not create any new Docker image nor affect production or other environments.
5. The manager can assign one or a few 'channel tags' to the merged applications in a simple text format file /tags.txt, and this will force the CI Pipeline to retag the app images in the Registry, if necessary.
6. Each k8s cluster has an assigned 'channel tag' at install time, and each cluster constantly queries the Registry for the applications with the same tag.
7. Once the application version change is detected, the cluster 'deployer' like ArgoCD downloads and replaces the running application image with a new one.

Init:

1. Commit to 'master' branch with commit message 'kanoko init'
2. Wait for pipeline well done
