//-----GPL----------------------------------------------------------------------
//
// This file is part of AdVersion
// Copyright (C) 2015  Zach Cobell
//
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------
//
//  File: AdVersion_static.cpp
//
//------------------------------------------------------------------------------
#include "AdVersion.h"
#include "git2.h"
#include <float.h>
#include <QtMath>
#include <QStringList>
#include <stdlib.h>



//-----------------------------------------------------------------------------------------//
//...Static method to retrieve the Git version form a repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Static method to return the Git version of the HEAD in a Git repository
 *
 * @param[in]  gitDirectory Location of the Git repository
 * @param[out] version      Current revision of the Git repository
 *
 * Static method to return the Git version fo the HEAD in a Git repository.
 * Equivalent to: git describe --always --tags
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::getGitVersion(QString gitDirectory, QString &version)
{
    int ierr;
    QFile dir(gitDirectory+"/.git");
    QByteArray tempData;
    git_repository *repo;
    git_describe_result *description;
    git_describe_options options;
    git_describe_format_options format;
    git_object *headObject;
    git_buf buffer = { 0 };

    version = QString();

    if(!dir.exists())
        return -1;

    tempData = gitDirectory.toLatin1();
    const char *cgitDirectory = tempData.data();

    git_libgit2_init();

    ierr = git_repository_open(&repo,cgitDirectory);
    if(ierr<0)
    {
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_describe_init_options(&options,GIT_DESCRIBE_OPTIONS_VERSION);
    options.show_commit_oid_as_fallback = 1;
    if(ierr<0)
    {
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_describe_init_format_options(&format,GIT_DESCRIBE_FORMAT_OPTIONS_VERSION);
    if(ierr<0)
    {
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_revparse_single(&headObject,repo,"HEAD");
    if(ierr<0)
    {
        git_object_free(headObject);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_describe_commit(&description,headObject,&options);
    if(ierr<0)
    {
        git_object_free(headObject);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    ierr = git_describe_format(&buffer,description,&format);
    if(ierr<0)
    {
        git_object_free(headObject);
        git_describe_result_free(description);
        git_repository_free(repo);
        git_libgit2_shutdown();
        return ierr;
    }

    version.sprintf("%s",buffer.ptr);

    git_object_free(headObject);
    git_describe_result_free(description);
    git_repository_free(repo);
    git_libgit2_shutdown();

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Static method to initialize an empty Git repository
//-----------------------------------------------------------------------------------------//
/**
 * \brief Static method to initialize an empty Git repository
 *
 * @param[in]  gitDirectory Location of the Git repository
 *
 * Static method to initialize an empty Git repository
 *
 **/
//-----------------------------------------------------------------------------------------//
int AdVersion::gitInit(QString gitDirectory)
{

    int ierr;
    git_repository *repository;
    QByteArray tempData = gitDirectory.toLatin1();
    const char *cgitDirectory = tempData.data();

    git_libgit2_init();
    ierr = git_repository_init(&repository,cgitDirectory,0);
    git_repository_free(repository);
    git_libgit2_shutdown();
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
