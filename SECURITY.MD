Rumor has it that there are more vulnerabilities in Project64 1.6.2, of which have not had their technical workings explained yet. If you can confirm a vulnerability and explain the technical workings of a vulnerability present in Project64 1.6.2, please DM me on discord at bruceshankle or open an issue about it on GitHub and make sure I am notified so I am made aware of it, then we can start thinking about how it might be able to be fixed. However, if it is not addressed right away, do not fret, as Project64 1.6.2 has multiple failesafe mechanisms in the event of a vulnerability exploitation by a malicious ROM.

In addition to said failesafe mechanism's, it would be much more difficult to take advantage of any vulnerability in Project64 1.6.2 compared to the original Project64 1.6 due to 1.6.2 building with ALSR (which randomizes addresses).

What failesafe mechanism's does Project64 1.6.2 have exactly? I'll let GitHub tell you in its own words. But the key point is: "Overall, these measures help mitigate the impact of a vulnerability exploitation by containing the process's behavior and reducing its ability to perform harmful actions."





This code is used to create and configure a job object in Windows, which applies various restrictions and limits to the process it's assigned to. Here is a detailed explanation of what each part of the code does and how it enhances the security of the program:

1. **Creating a Job Object:**
   ```cpp
   HANDLE hJob = CreateJobObject(NULL, NULL);
   ```
   - This line creates a job object, which is a container that allows you to manage and limit the resources of one or more processes.

2. **Setting Basic UI Restrictions:**
   ```cpp
   JOBOBJECT_BASIC_UI_RESTRICTIONS jbur = { 0 };
   jbur.UIRestrictionsClass = JOB_OBJECT_UILIMIT_DESKTOP |
       JOB_OBJECT_UILIMIT_DISPLAYSETTINGS |
       JOB_OBJECT_UILIMIT_EXITWINDOWS |
       JOB_OBJECT_UILIMIT_GLOBALATOMS |
       JOB_OBJECT_UILIMIT_HANDLES |
       JOB_OBJECT_UILIMIT_READCLIPBOARD |
       JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS |
       JOB_OBJECT_UILIMIT_WRITECLIPBOARD;
   if (!SetInformationJobObject(hJob, JobObjectBasicUIRestrictions, &jbur, sizeof(jbur))) CloseHandle(hJob);
   ```
   - This block sets user interface restrictions on the job object, such as preventing the process from accessing the desktop, changing display settings, exiting Windows, accessing global atoms, and more. These restrictions help prevent the process from interacting with the user interface in potentially malicious ways.

3. **Setting Extended Limit Information:**
   ```cpp
   JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
   jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS |
       JOB_OBJECT_LIMIT_AFFINITY |
       JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION |
       JOB_OBJECT_LIMIT_JOB_MEMORY |
       JOB_OBJECT_LIMIT_PRIORITY_CLASS |
       JOB_OBJECT_LIMIT_PROCESS_MEMORY |
       JOB_OBJECT_LIMIT_WORKINGSET;
   jeli.BasicLimitInformation.ActiveProcessLimit = 1;
   jeli.JobMemoryLimit = 100 * 1024 * 1024;
   jeli.ProcessMemoryLimit = 50 * 1024 * 1024;
   jeli.BasicLimitInformation.MinimumWorkingSetSize = 10 * 1024 * 1024;
   jeli.BasicLimitInformation.MaximumWorkingSetSize = 50 * 1024 * 1024;
   jeli.BasicLimitInformation.Affinity = 1;
   jeli.BasicLimitInformation.PriorityClass = NORMAL_PRIORITY_CLASS;
   if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) CloseHandle(hJob);
   ```
   - This block sets various limits on the job object, such as limiting the number of active processes to 1, setting memory limits, enforcing process affinity (CPU core binding), and ensuring the process dies on unhandled exceptions. These limits help contain the process's resource usage and behavior, making it harder for an exploit to spread or consume excessive resources.

4. **Setting Security Limits:**
   ```cpp
   JOBOBJECT_SECURITY_LIMIT_INFORMATION jsl = { 0 };
   jsl.SecurityLimitFlags = JOB_OBJECT_SECURITY_NO_ADMIN |
       JOB_OBJECT_SECURITY_RESTRICTED_TOKEN |
       JOB_OBJECT_SECURITY_ONLY_TOKEN |
       JOB_OBJECT_SECURITY_FILTER_TOKENS;
   if (!SetInformationJobObject(hJob, JobObjectSecurityLimitInformation, &jsl, sizeof(jsl))) CloseHandle(hJob);
   ```
   - This block sets security limits on the job object, such as preventing the process from running with administrative privileges, using restricted tokens, and filtering tokens to only allow specific privileges. These security limits reduce the potential damage if the process is compromised.

5. **Assigning the Current Process to the Job Object:**
   ```cpp
   if (!AssignProcessToJobObject(hJob, GetCurrentProcess())) CloseHandle(hJob);
   ```
   - Finally, this line assigns the current process to the job object, applying all the configured restrictions and limits to the process.

**Security Improvements:**
- **UI Restrictions:** Prevent the process from interacting with the user interface in a way that could be used for malicious purposes.
- **Resource Limits:** Constrain the process's resource usage, preventing denial-of-service attacks by resource exhaustion.
- **Process Limits:** Limit the number of processes, preventing the spawning of multiple malicious processes.
- **Security Restrictions:** Reduce the process's privileges, making it harder for an attacker to escalate privileges or perform system-level actions.

Overall, these measures help mitigate the impact of a vulnerability exploitation by containing the process's behavior and reducing its ability to perform harmful actions.
